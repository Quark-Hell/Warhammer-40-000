// Warhammer 40 000.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <vector>
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
void Controller(vector<Unit> unitsForCheck) {
    for (size_t i = 0; i < unitsForCheck.size(); i++)
    {
        int CountSeeUnit = 0;
        for (size_t j = 0; j < unitsForCheck.size(); j++)
        {
            if (CheckView(unitsForCheck[i], unitsForCheck[j].Position)) {
                CountSeeUnit++;
            }
        }
        printf("Unit Number %i sees %i unit(s) \n", i, CountSeeUnit);
    }
} //Адская машина,которая отвечает за проверку видимости других юнитиов для каждого юнита

int main()
{
#pragma region Fucking Rand in C++
    float someSeed = 0;
    mt19937 gen(someSeed);
    uniform_int_distribution<int> dis(-9.0, 9.0);
#pragma endregion

#pragma region Create unit
    vector<Unit> unit;

#pragma region Add your value
    unit.push_back(Unit(
        { 1,1 },   //Position
        { 0,1 },  //DirectionView
        135.5,  //AngleView
        2));       //DistanceView

    unit.push_back(Unit(
        { 1,2 },   //Position
        { 1,0 },  //DirectionView
        135.5,  //AngleView
        2));         //DistanceView

    unit.push_back(Unit(
        { -5,-1 },   //Position
        { 0.707, 0.707 },  //DirectionView
        135.5,  //AngleView
        2));        //DistanceView
#pragma endregion 

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

    Controller(unit);
    //CheckView(unit1, unit2.Position);
    std::cout << "Hello World!\n";
}
