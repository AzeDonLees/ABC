#include <Windows.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <ctime>
#include <mutex>

using namespace std;

bool is_numeric(string const& str) {
	auto result = double();
	auto i = istringstream(str);
	i >> result;
	return !i.fail() && i.eof();
}

class Cashbox;
DWORD WINAPI pThreadAttach(LPVOID Param);

class ClientClass
{
public:

	ClientClass(bool _status, int _stopper, int _clientId) : status(_status), stopper(_stopper), clientId(_clientId)
	{
		this->stopper = stopper;
	}
	//задержка клиента
	int stopper = 0;
	//состояние - обработала ли его касса
	bool status = false;
	//случайный айди клиента чтобы различать их
	int clientId = 0;
};

class Cashbox
{
public:
	//Создаем поток кассы
	Cashbox(string _Name) : hThread(CreateThread(0, 0, pThreadAttach, reinterpret_cast<LPVOID>(this), 0, 0)), Name(_Name)
	{

	}
	mutex mtxClient;
	vector<ClientClass> Client;
	string Name;
public:
	HANDLE thread()
	{
		return this->hThread;
	}
	bool Can()
	{
		return this->canWork;
	}
	void SetCan(bool bEnable)
	{
		this->canWork = bEnable;
	}
private:
	//дескриптор потока кассы
	HANDLE hThread = NULL;
	//переменная для работы цикла в потоке
	bool canWork = true;
};

DWORD WINAPI pThreadAttach(LPVOID Param)
{
	Cashbox* Box = (Cashbox*)Param;
	bool HasClientOnList = false;
	bool signal = false;

	while (Box->Can())
	{
		Box->mtxClient.lock();

		auto client_list = Box->Client;

		//Если нет клиентов то скип
		if (client_list.empty())
		{
			Box->mtxClient.unlock();
			if (!signal)
			{
				signal = !signal;
				cout << Box->Name << " спит...\n";
			}
			Sleep(150);
			continue;
		}
		else
		{
			signal = false;
			cout << Box->Name << " получила " << client_list.size() << " покупателей" << endl;

			for (auto client : client_list)
			{
				if (!client.status)
				{
					if (client.stopper)
					{
						//задержка клиента
						Sleep(client.stopper);
					}
					client.status = true;
					cout << "Клиент №" << client.clientId << " обработан : " << Box->Name << endl;
				}
			}

			Box->Client.clear();

		}

		Box->mtxClient.unlock();
	}

	return 0;
}

int main()
{
	//Для русификации консоли
	setlocale(0, "");
	//Состояние работы приложения
	bool state = false;
	//Действия цикла
	string whileMode;
	//Прием количества покупателей
	string peopleNoText;
	int peopleNoDigits = 0;
	//Инициализуем кассу
	Cashbox* pFirst = new Cashbox("Касса 1");
	Cashbox* pSecond = new Cashbox("Касса 2");
	Sleep(800);
	do
	{
		pFirst->mtxClient.lock();
		bool bFirstList = pFirst->Client.empty();
		pFirst->mtxClient.unlock();

		pSecond->mtxClient.lock();
		bool bSecondList = pSecond->Client.empty();
		pSecond->mtxClient.unlock();

		if (!bFirstList || !bSecondList)
		{
			Sleep(200);
			continue;
		}

		//Класс покупателей
		vector<ClientClass> mBox;
		//Клиенты первой кассы
		vector<ClientClass> ClassFirst;
		//Клиенты второй кассы
		vector<ClientClass> ClassNext;

		cout << "\nВведите 's' для старта или для выхода введите 'e': " << endl;
		cin >> whileMode;
		//Проверки что ввели
		if (whileMode._Equal("s") || whileMode._Equal("S"))
		{
			while (true)
			{
				cout << "Введите количество покупателей: " << endl;
				cin >> peopleNoText;
				if (is_numeric(peopleNoText) && atoi(peopleNoText.data()) > 0)
				{
					//Сохраняю число клиентов
					peopleNoDigits = atoi(peopleNoText.data());
					break;
				}
				else
					cout << "Неверный формат, введите число!" << endl;
			}
			//Раскидываем клиентов в лист
			for (int index = 0; index < peopleNoDigits; index++)
				mBox.push_back(ClientClass(false, rand() % 60 + 1, rand() % 500 + 1));

			//Кидаем покупашек кассирам
			for (auto& atom : mBox)
			{
				int firstOrNext = rand() % 2 + 1;
				if (firstOrNext == 1)
				{
					ClassFirst.push_back(atom);
				}
				else
				{
					ClassNext.push_back(atom);
				}
			}
			//Загружаем покупашек
			lock_guard<mutex> lock_first(pFirst->mtxClient);
			pFirst->Client.swap(ClassFirst);
			lock_guard<mutex> lock_second(pSecond->mtxClient);
			pSecond->Client.swap(ClassNext);




		}
		else if (whileMode._Equal("e") || whileMode._Equal("E"))
		{
			state = true;
		}
		else
			cout << "Ошибка ввода повторите еще раз!" << endl;

		//Чистим контенер и переменную
		peopleNoDigits = 0;
		mBox.clear();
		ClassFirst.clear();
		ClassNext.clear();


	} while (!state);

	//чтобы вышли из цикла
	pFirst->SetCan(false);
	pSecond->SetCan(false);
	//7 раз new 7 раз delete...
	delete pFirst;
	delete pSecond;

	cout << "Успешный выход, из программы!" << endl;
	return 0;
}