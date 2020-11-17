#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <future>
#include <thread>
#include <mutex>

using namespace std;
#define ILINE _inline
enum class type_zero { ZERO };

struct Vec3 {
    ILINE Vec3(type_zero) : x(0), y(0), z(0) {}
    ILINE Vec3(double x, double y, double z) : x(x), y(y), z(z) {}
    ILINE Vec3() { Vec3(0, 0, 0); }
    double x, y, z;
};

//Проверка цифра ли
bool is_numeric(string const& str) {
    auto result = double();
    auto i = istringstream(str);

    i >> result;

    return !i.fail() && i.eof();
}

//Проверка на компларность
string Compl(Vec3 vecx, Vec3 vecy, Vec3 vecz)
{
    return ((vecx.x * vecy.y * vecz.z + vecy.x * vecz.y * vecx.z + vecx.y * vecy.z * vecz.x - vecx.z * vecy.y * vecz.x - vecz.y * vecy.z * vecx.x - vecx.y * vecy.x * vecz.z) == 0) ? "Компланарный" : "Не компланарный";
}

bool create = false;
std::mutex listMtx;

//Данные векторов
vector<Vec3> vectorsData;

int main()
{
    //Русификация консоли
    setlocale(0, "");

    std::future<void> update_async = std::async(std::launch::async, []()
        {
            //Переменные для определения Х, Y, Z
            string x, y, z;
            //Переменная для определения кол-ва векторов
            string vecCount;
            //Данные векторов
            while (true)
            {
                //Определяем кол-во векторов
                cout << "Введите количество векторов: ";
                cin >> vecCount;
                //Проверка правильно ли ввели значение
                if (is_numeric(vecCount))
                {
                    cout << "Количество введенных векторов для заполнения: " << vecCount.data() << endl;
                    break;
                }
                else
                    cout << "Неверный формат, введите число!" << endl;
            }

            if (atoi(vecCount.data()) < 3)
                vecCount = "3";

            //Заполняем вектор
            for (int i = 0; i < atoi(vecCount.data()); i++)
            {
                cout << "Введите (х, y, z): " << endl;
                cin >> x;
                cin >> y;
                cin >> z;

                //Проверка правильности ввода
                if (is_numeric(x) && is_numeric(y) && is_numeric(z))
                {
                    //заполняем массив
                    vectorsData.push_back(Vec3(atof(x.data()), atof(y.data()), atof(z.data())));
                    cout << "Вектор номер " << i + 1 << " успешно записан." << endl;
                }
                else
                {
                    //если что то не верно то идем на один шаг назад
                    cout << "Неверный формат при записе в вектор, введите число!" << endl;
                    i--;
                }
            }

            std::lock_guard<std::mutex> lock(listMtx);
            create = true;

        });

    update_async.get();


    listMtx.lock();
    if (create)
    {
        //Проверяем Компланарный или нет выводим
        for (int i = 0; i < vectorsData.size(); i++)
        {
            for (int j = i + 1; j < vectorsData.size(); j++)
            {
                for (int k = j + 1; k < vectorsData.size(); k++)
                {
                    string combData = " Vec3(" + to_string((int)vectorsData[i].x) + ", " + to_string((int)vectorsData[i].y) + ", " + to_string((int)vectorsData[i].z) + ") | " +
                        "Vec3(" + to_string((int)vectorsData[j].x) + ", " + to_string((int)vectorsData[j].y) + ", " + to_string((int)vectorsData[j].z) + ") | " +
                        "Vec3(" + to_string((int)vectorsData[k].x) + ", " + to_string((int)vectorsData[k].y) + ", " + to_string((int)vectorsData[k].z) + ")";

                    cout << "Комбинация " << combData << " = " << Compl(vectorsData[i], vectorsData[j], vectorsData[k]) << endl;
                }
            }
        }
        listMtx.unlock();
        std::cin.get();
    }
    listMtx.unlock();
    std::cin.get();

    return 0;
}