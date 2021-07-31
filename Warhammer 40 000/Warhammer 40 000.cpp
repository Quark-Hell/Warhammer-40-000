#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <ctime>
#include <random>


using namespace std;

struct FVector
{
    float X;
    float Y;

    float magnitude() {
        return sqrt((X * X) + (Y * Y));
    }
};//Назвал так-же,как в UE4
class Unit {
public:
    FVector Position;
    FVector DirectionView;

    float AngleView;
    float DistanceView;

public:
    Unit(FVector position = { 0,0 }, FVector directionView = { 0,0 }, float angleView = 0, float distanceView = 0) {
        Position = position;
        DirectionView = directionView;

        AngleView = angleView;
        DistanceView = distanceView;
    }
};

vector<Unit> unit;

std::ifstream::pos_type filesize(const char* filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}
void SaveDataUnits() {
    for (size_t i = 0; i < unit.size(); i++)
    {

        string info =
            to_string(unit[i].Position.X)
            + "\n"
            + to_string(unit[i].Position.Y)
            + "\n"
            + to_string(unit[i].DirectionView.X)
            + "\n"
            + to_string(unit[i].DirectionView.Y)
            + "\n"
            + to_string(unit[i].AngleView)
            + "\n"
            + to_string(unit[i].DistanceView)
            + "\n";

        std::ofstream out;
        out.open("InfoAboutUnits.txt", std::ios::app);
        if (out.is_open())
        {
            out << unit[i].Position.X << " " << unit[i].Position.Y << " " << unit[i].DirectionView.X << " " << unit[i].DirectionView.Y << " " << unit[i].AngleView << " " << unit[i].DistanceView << std::endl;
        }
    }
}
void LoadDataUnits() {
    float PosX;
    float PosY;

    float DirViewX;
    float DirViewY;

    float AngleView;
    float Distance;

    std::ifstream in("InfoAboutUnits.txt"); 
    if (in.is_open())
    {
        while (in >> PosX >> PosY >> DirViewX >> DirViewY >> AngleView >> Distance)
        {
            unit.push_back(Unit(
                { PosX, PosY },                //Position
                { DirViewX, DirViewY },  //DirectionView
                AngleView,                     //AngleView
                Distance));                     //DistanceView));
        }
    }
}
bool CheckView(Unit unit, FVector targetPos) {
    FVector Vector =
    { targetPos.X - unit.Position.X,
      targetPos.Y - unit.Position.Y };//Вычисление вектора до другого юнита

    if (Vector.magnitude() <= unit.DistanceView) { //Вычисление дистанции до цели ради преждевременного отсечения целей,которые выходят за пределы видимости
#pragma region Calculacte angle between 2 vectors
        float dot = (unit.DirectionView.X * Vector.X) + (unit.DirectionView.Y * Vector.Y);
        float det = (unit.DirectionView.X * Vector.Y) - (unit.DirectionView.Y * Vector.X);

        float angle = atan2(det, dot) * (180.0 / 3.141592653589793238463);//вычисление угла между векторами в радианах и последующяя конвертация в градусы
#pragma endregion
        if (angle < unit.AngleView / 2) {
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}
void Controller(int start,int end) {
    for (size_t i = start; i < end; i++)
    {
        int CountSeeUnit = 0;
        for (size_t j = 0; j < unit.size(); j++)
        {
            if (CheckView(unit[i], unit[j].Position)) {
                CountSeeUnit++;
            }
        }
        printf("Unit Number %i sees %i unit(s) \n", i, CountSeeUnit);
    }
} //Адская машина,которая отвечает за проверку видимости других юнитиов для каждого юнита
void AddUnit()
{
#pragma region Fucking Rand in C++
    float someSeed = 0;
    mt19937 gen(someSeed);
    uniform_int_distribution<int> dis(-9.0, 9.0);
#pragma endregion

#pragma region Create unit

    //Add 10 000 units
    for (size_t i = 0; i < 10000; i++)
    {
        unit.push_back(Unit(
            { (float)dis(gen), (float)dis(gen) },   //Position
            { (float)dis(gen), (float)dis(gen) },  //DirectionView
            135.5,  //AngleView
            2));       //DistanceView));
    }

#pragma endregion
}

int main()
{
    float start_time = clock(); // начальное время

    //Создаёт набор юнитов и сохраняет данные о них,если файл сохранения пустой.Иначе загружает из него данные о юнитах
    if (filesize("InfoAboutUnits.txt") == 0) {
        AddUnit();
        SaveDataUnits();
    }
    else
    {
        LoadDataUnits();
    }

    if (unit.size() < 2000) {
        thread t1(Controller, 0, unit.size());
        t1.join();
    }
    else if (unit.size() < 4000) {
        thread t1(Controller, 0, unit.size() / 2);
        thread t2(Controller, unit.size() / 2, unit.size());

        t1.join();
        t2.join();
    }
    else if (unit.size() < 6000) {
        thread t1(Controller, 0, unit.size() / 3);
        thread t2(Controller, unit.size() / 3, unit.size() / 3 * 2);
        thread t3(Controller, unit.size() / 3 * 2, unit.size());

        t1.join();
        t2.join();
        t3.join();
    }
    else if (unit.size() < 8000) {
        thread t1(Controller, 0, unit.size() / 4);
        thread t2(Controller, unit.size() / 4, unit.size() / 4 *2);
        thread t3(Controller, unit.size() / 4 * 2, unit.size() / 4 * 3);
        thread t4(Controller, unit.size() / 4 * 3, unit.size());

        t1.join();
        t2.join();
        t3.join();
        t4.join();
    }
    else {
        thread t1(Controller, 0,                           unit.size() / 5);
        thread t2(Controller, unit.size() / 5,       unit.size() / 5 * 2);
        thread t3(Controller, unit.size() / 5 * 2, unit.size() / 5 * 3);
        thread t4(Controller, unit.size() / 5 * 3, unit.size() / 5 * 4);
        thread t5(Controller, unit.size() / 5 * 4, unit.size());

        t1.join();
        t2.join();
        t3.join();
        t4.join();
        t5.join();
    }

    float end_time = clock(); // конечное время
    float search_time = end_time - start_time; // искомое время

    printf("%f", search_time);
}
