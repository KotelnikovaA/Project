#include <iostream>
#include <iomanip>
#include <vector>
#include "DirectSearch.h"
#include "Rastrigin.h"
#include "HillFunction.h"

using namespace std;

void printResult(const Point& result) {
    cout << fixed << setprecision(8);
    cout << "Значение функции: " << result.value << "\n";
    cout << "Точка минимума: (";

    for (size_t i = 0; i < result.point.size(); i++) {
        cout << result.point[i];
        if (i < result.point.size() - 1) cout << ", ";
    }
    cout << ")\n";
}

void demoRastrigin(int dimension) {
    cout << "\n" << string(60, '=') << "\n";
    cout << "ФУНКЦИЯ РАСТРИГИНА (" << dimension << "D)\n";
    cout << string(60, '=') << "\n";

    DirectSearch searcher;
    vector<double> lower, upper;
    getRastriginBounds(dimension, lower, upper);

    double step = 0.1;

    cout << "Область поиска:\n";
    for (int i = 0; i < dimension; i++) {
        cout << "  x" << i << " в [" << lower[i] << ", " << upper[i] << "]\n";
    }
    cout << "Шаг сетки: " << step << "\n\n";

    Point result = searcher.findMinimum(lower, upper, step, rastrigin);

    cout << "РЕЗУЛЬТАТ ПОИСКА:\n";
    printResult(result);

    vector<double> optimum = getRastriginOptimum(dimension);
    double optimumValue = rastrigin(optimum);
}

void demoHill(int problemIndex) {
    cout << "\n" << string(60, '=') << "\n";
    cout << "ФУНКЦИЯ ХИЛЛА (задача #" << problemIndex << ")\n";
    cout << string(60, '=') << "\n";

    DirectSearch searcher;

    vector<double> lower = { 0.0 };
    vector<double> upper = { 1.0 };
    double step = 0.001;

    cout << "Область поиска: x в [" << lower[0] << ", " << upper[0] << "]\n";
    cout << "Шаг сетки: " << step << "\n";

    double knownOptimumValue;
    vector<double> knownOptimum = getHillOptimum(problemIndex, knownOptimumValue);

    cout << "Известный минимум: f(" << knownOptimum[0] << ") = " << knownOptimumValue << "\n";

    auto hillFunc = [problemIndex](const vector<double>& x) -> double {
        return hillFunction(problemIndex, x);
        };

    Point result = searcher.findMinimum(lower, upper, step, hillFunc);

    cout << "РЕЗУЛЬТАТ ПОИСКА:\n";
    printResult(result);
}

int main() {
    setlocale(LC_ALL, "Russian");

    cout << "МЕТОД ПРЯМОГО ПЕРЕБОРА ДЛЯ ТЕСТОВЫХ ФУНКЦИЙ\n";
    cout << "===========================================\n";

    int choice;
    do {
        cout << "\nМЕНЮ ТЕСТИРОВАНИЯ:\n";
        cout << "1. Функция Растригина (2D)\n";
        cout << "2. Функция Растригина (3D)\n";
        cout << "3. Функция Хилла (задача 0)\n";
        cout << "4. Функция Хилла (задача 42)\n";
        cout << "0. Выход\n";
        cout << "Выберите тест: ";
        cin >> choice;

        switch (choice) {
        case 1:
            demoRastrigin(2);
            break;
        case 2:
            demoRastrigin(3);
            break;
        case 3:
            demoHill(0);
            break;
        case 4:
            demoHill(42);
            break;
        case 0:
            cout << "Выход из программы.\n";
            break;
        default:
            cout << "Неверный выбор. Попробуйте снова.\n";
        }
    } while (choice != 0);

    return 0;
}