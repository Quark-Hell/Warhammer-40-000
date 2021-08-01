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

class Rectangle {
public:
    float X;
    float Y;
    float W;
    float H;
    Rectangle(float x = 0, float y = 0, float w = 0, float h = 0) {
        X = x;
        Y = y;
        W = w;
        H = h;
    }

    bool Contains(Unit unit) {
        return (
            unit.Position.X >= X - W &&
            unit.Position.X < X + W &&
            unit.Position.Y >= Y - H &&
            unit.Position.Y < Y + H
            );
    }

    bool Intersects(Rectangle range) {
        return !(
            range.X - range.W > X + W ||
            range.X + range.W <X - W ||
            range.Y - range.H > Y + H ||
            range.Y + range.H < Y - H
            );
    }
};

class QuadTree {
    int Capacity;

public:
    Rectangle Boundary;
    vector<Unit> Points;
    bool Divided;

    QuadTree* NorthWest;
    QuadTree* NorthEast;
    QuadTree* SouthWest;
    QuadTree* SouthEast;

    QuadTree(Rectangle boundary, int capacity) {
        Boundary = boundary;
        Capacity = capacity;
        Divided = false;
    }

    void subdivide() {
        auto x = Boundary.X;
        auto y = Boundary.Y;
        auto w = Boundary.W;
        auto h = Boundary.H;
        auto ne = Rectangle(x + w / 2, y - h / 2, w / 2, h / 2);
        NorthEast = new QuadTree(ne, Capacity);
        auto nw = Rectangle(x - w / 2, y - h / 2, w / 2, h / 2);
        NorthWest = new QuadTree(nw, Capacity);
        auto se = Rectangle(x + w / 2, y + h / 2, w / 2, h / 2);
        SouthEast = new QuadTree(se, Capacity);
        auto sw = Rectangle(x - w / 2, y + h / 2, w / 2, h / 2);
        SouthWest = new QuadTree(sw, Capacity);
        Divided = true;
    }

    bool insert(Unit unit) {
        if (!Boundary.Contains(unit)) {
            return false;
        }

        if (Points.size() < Capacity) {
            Points.push_back(unit);
            return true;
        }
        else {
            if (NorthWest == nullptr) {
                subdivide();
            }
            if (NorthWest->insert(unit)) {
                return true;
            }
            else if (NorthEast->insert(unit)) {
                return true;
            }
            else if (SouthWest->insert(unit)) {
                return true;
            }
            else if (SouthEast->insert(unit)) {
                return true;
            }
        }
    }

    void query(Rectangle range, vector<Unit>& found) {
        if (!Boundary.Intersects(range)) {
            return;
        }
        else {
            for (size_t i = 0; i < Points.size(); i++) {
                if (range.Contains(Points[i])) {
                    found.push_back(Points[i]);
                }
            }
            if (NorthWest == nullptr) {
                return;
            }
            if (Divided) {
                NorthWest->query(range, found);
                NorthEast->query(range, found);
                SouthWest->query(range, found);
                SouthEast->query(range, found);
            }
        }
        return;
    }
};

Rectangle boundry = Rectangle(0, 0, 10, 10);
QuadTree quadTree = QuadTree(boundry, 20);

void Setup() {
    for (size_t i = 0; i < unit.size(); i++)
    {
        quadTree.insert(unit[i]);
    }
}
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
        if (angle > -unit.AngleView / 2 && angle < unit.AngleView / 2) {
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}
void Controller(int start, int end) {
    int CountSeeUnit = 0;
    for (size_t i = start; i < end; i++)
    {
        Rectangle range = Rectangle(unit[i].Position.X, unit[i].Position.Y, unit[i].DistanceView * 2, unit[i].DistanceView * 2);

        vector<Unit> unitInRangeView;
        quadTree.query(range, unitInRangeView);//Все юниты,которые находятся в радиусе обзора юнита

        CountSeeUnit = 0;
        for (size_t j = 0; j < unitInRangeView.size(); j++)
        {
            if (CheckView(unit[i], unitInRangeView[j].Position)) {
                CountSeeUnit++;
            }
        }
        printf("Unit Number %i sees %i unit(s) \n", i, CountSeeUnit - 1);
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
        Setup();
    }
    else
    {
        LoadDataUnits();
        Setup();
    }

    //Контроллирует проверку видимости юнитом других юнитов,а так-же разделяет на несколько потоков,если юнитов много
    //Controller(0, unit.size());
    if (unit.size() < 5000) {
        thread t1(Controller, 0, unit.size());
        t1.join();
    }
    else {
        //Controller(0, unit.size());

        thread t1(Controller, 0, unit.size() / 2);
        thread t2(Controller, unit.size() / 2, unit.size() / 2 * 2);

        t1.join();
        t2.join();
    }

    std::cout << "Hello World!\n";

    float end_time = clock(); // конечное время
    float search_time = end_time - start_time; // искомое время

    printf("%f", search_time);
}