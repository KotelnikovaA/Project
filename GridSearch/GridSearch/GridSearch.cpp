#include <iostream>
#include <cmath>
#include <limits>
#include <vector>
#include <iomanip>

using namespace std;

struct Point {
    double x, y;
    double value;
};

double Function(double x, double y) {
    return x * x * x * x + 8 * x * x * x - 6 * x * x - 72 * x;
}

Point GridSearch(double x_min, double x_max, double y_min, double y_max, double step_x, double step_y) {
    Point best_point;
    best_point.value = std::numeric_limits<double>::max();

    int steps_x = static_cast<int>((x_max - x_min) / step_x) + 1;
    int steps_y = static_cast<int>((y_max - y_min) / step_y) + 1;

    for (int i = 0; i < steps_x; i++) {
        double x = x_min + i * step_x;

        if (i == steps_x - 1) x = x_max;

        for (int j = 0; j < steps_y; j++) {
            double y = y_min + j * step_y;
            if (j == steps_y - 1) y = y_max;

            double current_value = Function(x, y);

            if (current_value < best_point.value) {
                best_point.x = x;
                best_point.y = y;
                best_point.value = current_value;
            }
        }
    }
    return best_point;
}

Point GridSearchTable(double x_min, double x_max, double y_min, double y_max, double step_x, double step_y) {
    Point best_point;
    best_point.value = std::numeric_limits<double>::max();

    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << "ТАБЛИЦА ЗНАЧЕНИЙ ФУНКЦИИ";
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << std::left << std::setw(8) << "№"
        << std::setw(15) << "x"
        << std::setw(15) << "y"
        << std::setw(25) << "f(x,y)";
    std::cout << "\n" << std::string(50, '-') << "\n";

    int steps_x = static_cast<int>((x_max - x_min) / step_x) + 1;
    int steps_y = static_cast<int>((y_max - y_min) / step_y) + 1;
    int point_counter = 0;

    for (int i = 0; i < steps_x; i++) {
        double x = x_min + i * step_x;
        if (i == steps_x - 1) x = x_max;

        for (int j = 0; j < steps_y; j++) {
            double y = y_min + j * step_y;
            if (j == steps_y - 1) y = y_max;

            double current_value = Function(x, y);
            point_counter++;

            if (current_value < best_point.value) {
                best_point.x = x;
                best_point.y = y;
                best_point.value = current_value;
            }

            std::cout << std::left << std::setw(8) << point_counter
                << std::fixed << std::setprecision(6)
                << std::setw(15) << x
                << std::setw(15) << y
                << std::setw(25) << current_value
                << "\n";
        }
    }
    std::cout << "\n" << std::string(50, '=') << "\n";
    return best_point;
}

void PrintSearchInfo(double x_min, double x_max, double y_min, double y_max,
    double step_x, double step_y) {
    std::cout << "\nПАРАМЕТРЫ ПОИСКА:";
    std::cout << "\n" << std::string(50, '-') << "\n";
    std::cout << "Область поиска:\n";
    std::cout << "  x в [" << x_min << ", " << x_max << "]\n";
    std::cout << "  y в [" << y_min << ", " << y_max << "]\n";
    std::cout << "Шаг сетки:\n";
    std::cout << "  dx = " << step_x << "\n";
    std::cout << "  dy = " << step_y << "\n";

    int steps_x = static_cast<int>((x_max - x_min) / step_x) + 1;
    int steps_y = static_cast<int>((y_max - y_min) / step_y) + 1;
}

struct AnalyticalSolution {
    double x;
    double y;
    double value;
};

AnalyticalSolution analyticalSolution() {
    AnalyticalSolution sol;
    sol.x = 1.75;
    sol.y = 0;
    sol.value = -92.12;
    return sol;
}

void calculateErrors(const Point& result, double step_x, double step_y) {
    AnalyticalSolution analytical = analyticalSolution();

    double max_grid_error_x = step_x / 2.0;
    double max_grid_error_y = step_y / 2.0;

    double actual_error_x = fabs(result.x - analytical.x);
    double actual_error_y = fabs(result.y - analytical.y);
    double actual_error_value = fabs(result.value - analytical.value);

    cout << "\nВЫЧИСЛЕНИЕ ПОГРЕШНОСТЕЙ:";
    std::cout << "\n" << std::string(50, '-') << "\n";

    cout << "1. Погрешность сетки (максимально возможная):\n";
    cout << "   По x: ±" << max_grid_error_x << endl;
    cout << "   По y: ±" << max_grid_error_y << endl;

    cout << "2. Фактические погрешности:\n";
    cout << fixed << setprecision(6);
    cout << "   По x: " << actual_error_x << endl;
    cout << "   По y: " << actual_error_y << endl;
    cout << "   По значению: " << actual_error_value << endl;
    cout << "\n";
}

int main() {
    setlocale(LC_ALL, "Russian");

    double x_min = 1.5, x_max = 2.0;
    double y_min = 0.0, y_max = 0.0;

    double step_x = 0.05;
    double step_y = 0.05;

    std::cout << "ПОИСК МИНИМУМА ФУНКЦИИ МЕТОДОМ ПРЯМОГО ПЕРЕБОРА";
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << "Функция: f(x,y) = x^4 + 8x^3 - 6x^2 - 72x\n";

    PrintSearchInfo(x_min, x_max, y_min, y_max, step_x, step_y);

    Point result_1 = GridSearchTable(x_min, x_max, y_min, y_max, step_x, step_y);

    std::cout << "\nРЕЗУЛЬТАТ:\n";
    std::cout << "Минимум функции: f(" << std::fixed << std::setprecision(6) << result_1.x << ", " << result_1.y << ") = " << result_1.value << std::endl;

    calculateErrors(result_1, step_x, step_y);

    return 0;
}