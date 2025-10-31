#include "pch.h"
using namespace System;
using namespace System::IO;
using namespace System::Collections::Generic;


public ref class Shape abstract
{
public:
    virtual double Area() = 0;
    virtual double Perimeter() = 0;
    virtual String^ Print() = 0;
    virtual String^ Serialize() = 0;
};


public ref class Circle : public Shape
{
private:
    double r;
public:
    Circle(double r) { if (r <= 0) throw gcnew ArgumentException("Радиус должен быть > 0"); this->r = r; }

    virtual double Area() override { 
        return Math::PI * r * r;
    }
    virtual double Perimeter() override { 
        return 2 * Math::PI * r; 
    }
    virtual String^ Print() override {
        return String::Format("Circle (r = {0:F2} | Area = {1:F2} | Perimeter = {2:F2})", r, Area(), Perimeter());
    }
    virtual String^ Serialize() override { return String::Format("Circle;{0}", r); }
};


public ref class Rectangle : public Shape
{
private:
    double w, h;
public:
    Rectangle(double w, double h) {
        if (w <= 0 || h <= 0) throw gcnew ArgumentException("Стороны должны быть > 0");
        this->w = w; this->h = h;
    }
    virtual double Area() override { 
        return w * h; 
    }
    virtual double Perimeter() override { 
        return 2 * (w + h); 
    }
    virtual String^ Print() override {
        return String::Format("Rectangle ({0:F2};{1:F2} | Area = {2:F2} | Perimeter = {3:F2})", w, h, Area(), Perimeter());
    }
    virtual String^ Serialize() override { return String::Format("Rectangle;{0};{1}", w, h); }
};


public ref class Triangle : public Shape
{
private:
    double a, b, c;
public:
    Triangle(double a, double b, double c) {
        if (a <= 0 || b <= 0 || c <= 0)
            throw gcnew ArgumentException("Стороны должны быть > 0");
        if (a + b <= c || a + c <= b || b + c <= a)
            throw gcnew ArgumentException("Неравенство треугольника не выполняется");
        this->a = a; this->b = b; this->c = c;
    }

    virtual double Perimeter() override { return a + b + c; }
    virtual double Area() override {
        double p = Perimeter() / 2;
        return Math::Sqrt(p * (p - a) * (p - b) * (p - c));
    }
    virtual String^ Print() override {
        return String::Format("Triangle ({0:F2};{1:F2};{2:F2} | Area = {3:F2} | Perimeter = {4:F2})",
            a, b, c, Area(), Perimeter());
    }
    virtual String^ Serialize() override { return String::Format("Triangle;{0};{1};{2}", a, b, c); }
};

Shape^ Deserialize(String^ line)
{
    array<String^>^ parts = line->Split(';');
    if (parts->Length < 2) throw gcnew FormatException("Введены не все параметры");

    String^ type = parts[0]->Trim()->ToLower();
    try {
        if (type == "circle" && parts->Length == 2)
            return gcnew Circle(Double::Parse(parts[1]));

        else if (type == "rectangle" && parts->Length == 3)
            return gcnew Rectangle(Double::Parse(parts[1]), Double::Parse(parts[2]));

        else if (type == "triangle" && parts->Length == 4)
            return gcnew Triangle(Double::Parse(parts[1]), Double::Parse(parts[2]), Double::Parse(parts[3]));

        else
            throw gcnew FormatException("Неверное количество параметров");
    }
    catch (Exception^ ex) {
        throw gcnew FormatException("Ошибка при разборе строки: " + ex->Message);
    }
}

int main(array<System::String^>^ args)
{
    String^ fileName = "shapes.txt";
    List<Shape^>^ shapes = gcnew List<Shape^>();

    if (File::Exists(fileName)) {
        Console::WriteLine("Чтение данных из файла {0}", fileName);
        for each (String ^ line in File::ReadAllLines(fileName)) {
            try {
                shapes->Add(Deserialize(line));
            }
            catch (Exception^ ex) {
                Console::WriteLine("! Пропуск строки: \"{0}\" — {1}", line, ex->Message);
            }
        }
    }

    if (shapes->Count == 0) {
        Console::WriteLine("Файл не найден или пуст.Рассчитывается результат по стандартным значениям.\n");
        shapes->Add(gcnew Circle(3));
        shapes->Add(gcnew Rectangle(4, 5));
        shapes->Add(gcnew Triangle(3, 4, 5));
        shapes->Add(gcnew Circle(1.5));
        shapes->Add(gcnew Rectangle(2, 7));
    }

    Console::WriteLine("Результат:");
    double totalArea = 0, totalPerim = 0;
    double sumTriArea = 0; int triCount = 0;

    for each (Shape ^ s in shapes) {
        Console::WriteLine(s->Print());
        totalArea += s->Area();
        totalPerim += s->Perimeter();

        Triangle^ t = dynamic_cast<Triangle^>(s);
        if (t != nullptr) {
            sumTriArea += t->Area();
            triCount++;
        }
    }

    Console::WriteLine("----------------------------------------------------------------");
    Console::WriteLine("Итог: Area={0:F2}, Perimeter={1:F2}", totalArea, totalPerim);
    if (triCount > 0)
        Console::WriteLine("Средняя площадь треугольников = {0:F2}", sumTriArea / triCount);

    List<String^>^ lines = gcnew List<String^>();
    for each (Shape ^ s in shapes)
        lines->Add(s->Serialize());
    File::WriteAllLines(fileName, lines);

    Console::WriteLine("\nСохранение в файл {0} произошло успешно", fileName);
    Console::ReadLine();
    return 0;
}
