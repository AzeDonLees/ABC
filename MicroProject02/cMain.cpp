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
	//�������� �������
	int stopper = 0;
	//��������� - ���������� �� ��� �����
	bool status = false;
	//��������� ���� ������� ����� ��������� ��
	int clientId = 0;
};

class Cashbox
{
public:
	//������� ����� �����
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
	//���������� ������ �����
	HANDLE hThread = NULL;
	//���������� ��� ������ ����� � ������
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

		//���� ��� �������� �� ����
		if (client_list.empty())
		{
			Box->mtxClient.unlock();
			if (!signal)
			{
				signal = !signal;
				cout << Box->Name << " ����...\n";
			}
			Sleep(150);
			continue;
		}
		else
		{
			signal = false;
			cout << Box->Name << " �������� " << client_list.size() << " �����������" << endl;

			for (auto client : client_list)
			{
				if (!client.status)
				{
					if (client.stopper)
					{
						//�������� �������
						Sleep(client.stopper);
					}
					client.status = true;
					cout << "������ �" << client.clientId << " ��������� : " << Box->Name << endl;
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
	//��� ����������� �������
	setlocale(0, "");
	//��������� ������ ����������
	bool state = false;
	//�������� �����
	string whileMode;
	//����� ���������� �����������
	string peopleNoText;
	int peopleNoDigits = 0;
	//������������ �����
	Cashbox* pFirst = new Cashbox("����� 1");
	Cashbox* pSecond = new Cashbox("����� 2");
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

		//����� �����������
		vector<ClientClass> mBox;
		//������� ������ �����
		vector<ClientClass> ClassFirst;
		//������� ������ �����
		vector<ClientClass> ClassNext;

		cout << "\n������� 's' ��� ������ ��� ��� ������ ������� 'e': " << endl;
		cin >> whileMode;
		//�������� ��� �����
		if (whileMode._Equal("s") || whileMode._Equal("S"))
		{
			while (true)
			{
				cout << "������� ���������� �����������: " << endl;
				cin >> peopleNoText;
				if (is_numeric(peopleNoText) && atoi(peopleNoText.data()) > 0)
				{
					//�������� ����� ��������
					peopleNoDigits = atoi(peopleNoText.data());
					break;
				}
				else
					cout << "�������� ������, ������� �����!" << endl;
			}
			//����������� �������� � ����
			for (int index = 0; index < peopleNoDigits; index++)
				mBox.push_back(ClientClass(false, rand() % 60 + 1, rand() % 500 + 1));

			//������ ��������� ��������
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
			//��������� ���������
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
			cout << "������ ����� ��������� ��� ���!" << endl;

		//������ �������� � ����������
		peopleNoDigits = 0;
		mBox.clear();
		ClassFirst.clear();
		ClassNext.clear();


	} while (!state);

	//����� ����� �� �����
	pFirst->SetCan(false);
	pSecond->SetCan(false);
	//7 ��� new 7 ��� delete...
	delete pFirst;
	delete pSecond;

	cout << "�������� �����, �� ���������!" << endl;
	return 0;
}