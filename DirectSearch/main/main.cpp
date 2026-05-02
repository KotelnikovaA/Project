#include <iostream>
#include <iomanip>
#include <vector>
#include <functional>
#include <chrono>
#include <numeric>
#include <cmath>

#include "DiffEvolution.h"
#include "DirectSearch.h"
#include "Rastrigin.h"
#include "HillFunction.h"
#include "HillProblem.h"
#include "ShekelFunctions.h"
#include "GrishaginFunctions.h"
#include "GKLSFunctions.h"

using namespace std;

struct TableRow {
    vector<double> point;
    double value;
};

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

void printDEResult(const Individual& result) {
    cout << fixed << setprecision(8);
    cout << "Значение функции: " << result.functionVal << "\n";
    cout << "Точка минимума: (";

    for (size_t i = 0; i < result.genes.size(); i++) {
        cout << result.genes[i];
        if (i < result.genes.size() - 1) cout << ", ";
    }

    cout << ")\n";
}

// запуск прямого перебора 
Point runDirectSearch(
    const vector<double>& lower,
    const vector<double>& upper,
    double step,
    const function<double(const vector<double>&)>& func)
{
    DirectSearch searcher;
    return searcher.findMinimum(lower, upper, step, func);
}

// запуск запуск дифф эволюции
Individual runDE(
    const vector<double>& lower,
    const vector<double>& upper,
    int NP,
    double F,
    double CR,
    int M,
    const function<double(const vector<double>&)>& func,
    double knownOptimum)
{
    DifferentialEvolution de(
        NP,
        F,
        CR,
        M,
        lower,
        upper,
        func,
        knownOptimum,
        1e-12
    );

    return de.optimize();
}

//  РАСТРИГИН  //

// растригин для прямого перебора
void demoRastriginDirect(int dimension) {

    cout << "\nФУНКЦИЯ РАСТРИГИНА (" << dimension << "D)\n";

    vector<double> lower, upper;
    getRastriginBounds(dimension, lower, upper);

    double step = 0.2;

    Point result = runDirectSearch(lower, upper, step, rastrigin);

    printResult(result);

    vector<double> optimum = getRastriginOptimum(dimension);
    double optimumValue = rastrigin(optimum);

    cout << "Известный минимум: " << optimumValue << "\n";
    cout << "Ошибка: " << fabs(result.value - optimumValue) << "\n";
}

// растригин для дифф эволюции
void demoRastriginDE(int dimension) {

    cout << "\nDE - ФУНКЦИЯ РАСТРИГИНА (" << dimension << "D)\n";

    vector<double> lower, upper;
    getRastriginBounds(dimension, lower, upper);

    vector<double> optimum = getRastriginOptimum(dimension);
    double optimumValue = rastrigin(optimum);

    Individual result = runDE(
        lower,
        upper,
        50,
        0.8,
        0.9,
        200,
        rastrigin,
        optimumValue
    );

    printDEResult(result);

    cout << "Известный минимум: " << optimumValue << "\n";
    cout << "Ошибка: " << fabs(result.functionVal - optimumValue) << "\n";
}

//  ХИЛЛ  //

// хилл для прямого перебора
void demoHillDirect(int problemIndex) {

    cout << "\nФУНКЦИЯ ХИЛЛА #" << problemIndex << "\n";

    vector<double> lower = { 0.0 };
    vector<double> upper = { 1.0 };

    double knownValue;
    vector<double> optimum = getHillOptimum(problemIndex, knownValue);

    auto func = [problemIndex](const vector<double>& x) {
        return hillFunction(problemIndex, x);
        };

    Point result = runDirectSearch(lower, upper, 0.001, func);

    printResult(result);

    cout << "Известный минимум: " << knownValue << "\n";
    cout << "Ошибка: " << fabs(result.value - knownValue) << "\n";
}

// хилл для дифф эволюции
void demoHillDE(int problemIndex) {

    cout << "\nDE - ФУНКЦИЯ ХИЛЛА #" << problemIndex << "\n";

    vector<double> lower = { 0.0 };
    vector<double> upper = { 1.0 };

    double knownValue;
    vector<double> optimum = getHillOptimum(problemIndex, knownValue);

    auto func = [problemIndex](const vector<double>& x) {
        return hillFunction(problemIndex, x);
        };

    Individual result = runDE(
        lower,
        upper,
        20,
        0.8,
        0.9,
        200,
        func,
        knownValue
    );

    printDEResult(result);

    cout << "Известный минимум: " << knownValue << "\n";
    cout << "Ошибка: " << fabs(result.functionVal - knownValue) << "\n";
}

// весь хилл для прямого перебора
void testHillDirectAll() {

    cout << "\nТЕСТ 1000 ЗАДАЧ ХИЛЛА (ПРЯМОЙ ПЕРЕБОР)\n";

    vector<double> lower = { 0.0 };
    vector<double> upper = { 1.0 };
    double step = 0.001;

    int success = 0;
    long long totalEval = 0;
    double totalTime = 0.0;

    for (int i = 0; i < NUM_HILL_PROBLEMS; i++) {

        double knownValue;
        getHillOptimum(i, knownValue);

        auto func = [i](const vector<double>& x) {
            return hillFunction(i, x);
            };

        auto start = chrono::high_resolution_clock::now();

        Point result = runDirectSearch(lower, upper, step, func);

        auto end = chrono::high_resolution_clock::now();
        double timeMs = chrono::duration<double, milli>(end - start).count();

        totalTime += timeMs;
        totalEval += 1001; 

        if (fabs(result.value - knownValue) <= 0.01)
            success++;

        if ((i + 1) % 100 == 0)
            cout << "Обработано " << i + 1 << " задач...\n";
    }

    double avgTime = totalTime / NUM_HILL_PROBLEMS;
    double avgEval = static_cast<double>(totalEval) / NUM_HILL_PROBLEMS;
    double successRate = 100.0 * success / NUM_HILL_PROBLEMS;

    cout << "\n" << string(60, '-') << "\n";
    cout << "РЕЗУЛЬТАТЫ ТЕСТИРОВАНИЯ ПРЯМОГО ПЕРЕБОРА:\n";
    cout << string(60, '-') << "\n";
    cout << fixed << setprecision(6);
    cout << "Шаг сетки: " << step << "\n";
    cout << "Точек сетки на задачу: " << 1001 << "\n";
    cout << "Успешно решено: " << success << "/" << NUM_HILL_PROBLEMS
        << " (" << successRate << "%)\n";
    cout << "Среднее время на задачу: " << avgTime << " мс\n";
    cout << "Среднее количество вычислений на задачу: " << (int)avgEval << "\n";
}

// весь хилл для диф эволюции
void testHillDEAll() {

    cout << "\nТЕСТ 1000 ЗАДАЧ ХИЛЛА (DE, МНОГОКРАТНЫЕ ЗАПУСКИ)\n";

    vector<double> lower = { 0.0 };
    vector<double> upper = { 1.0 };

    int NP = 40;
    double F = 0.7;
    double CR = 0.85;
    int M = 150;
    int maxRuns = 5;

    int success = 0;
    long long totalEval = 0;
    double totalTime = 0.0;
    int totalRuns = 0;

    auto startTotal = chrono::high_resolution_clock::now();

    for (int i = 0; i < NUM_HILL_PROBLEMS; i++) {

        double knownValue;
        getHillOptimum(i, knownValue);

        auto func = [i](const vector<double>& x) {
            return hillFunction(i, x);
            };

        Individual bestResult;
        bestResult.functionVal = numeric_limits<double>::max();

        int runsUsed = 0;
        int bestEvals = 0;

        for (int run = 0; run < maxRuns; run++) {
            runsUsed++;

            DifferentialEvolution de(
                NP, F, CR, M,
                lower, upper,
                func,
                knownValue,
                0.01
            );

            Individual result = de.optimize();

            if (result.functionVal < bestResult.functionVal) {
                bestResult = result;
                bestEvals = de.getEvaluations();
            }

            if (fabs(bestResult.functionVal - knownValue) <= 0.01) {
                break;
            }
        }

        totalEval += bestEvals;
        totalRuns += runsUsed;

        if (fabs(bestResult.functionVal - knownValue) <= 0.01)
            success++;

        if ((i + 1) % 100 == 0)
            cout << "Обработано " << i + 1 << " задач...\n";
    }

    auto endTotal = chrono::high_resolution_clock::now();
    double totalTimeMs = chrono::duration<double, milli>(endTotal - startTotal).count();

    cout << "\n" << string(60, '-') << "\n";
    cout << "РЕЗУЛЬТАТЫ:\n";
    cout << string(60, '-') << "\n";
    cout << fixed << setprecision(6);
    cout << "Параметры: NP=" << NP << ", F=" << F << ", CR=" << CR << ", M=" << M << "\n";
    cout << "Максимум запусков: " << maxRuns << "\n";
    cout << "Успешно решено: " << success << "/" << NUM_HILL_PROBLEMS
        << " (" << (100.0 * success / NUM_HILL_PROBLEMS) << "%)\n";
    cout << "Среднее время на задачу: " << totalTimeMs / NUM_HILL_PROBLEMS << " мс\n";
    cout << "Среднее количество вычислений на задачу: " << totalEval / NUM_HILL_PROBLEMS << "\n";
    cout << "Среднее количество запусков на задачу: " << (double)totalRuns / NUM_HILL_PROBLEMS << "\n";
}

// весь хилл для диф эволюции с сохранением инфы в exel
#include <fstream>
#include <sstream>

void testHillDEAllWithAllStatistic() {
    cout << "\nТЕСТ 100 ЗАДАЧ ХИЛЛА (DE) С СОХРАНЕНИЕМ РЕЗУЛЬТАТОВ\n";

    vector<double> lower = { 0.0 };
    vector<double> upper = { 1.0 };

    int NP = 40;
    double F = 0.7;
    double CR = 0.85;
    int M = 150;
    int maxRuns = 5;

    string filename = "hill_results_de.csv";
    ofstream outFile(filename);
    outFile << "Номер задачи;Найденное значение;Известное значение;Ошибка;"
        << "Вычислений;Время(мс);Количество запусков;Успех\n";
    outFile.close();

    int success = 0;
    long long totalEval = 0;
    double totalTime = 0.0;
    int totalRuns = 0;

    for (int i = 0; i < 1000; i++) {
        double knownValue;
        getHillOptimum(i, knownValue);

        auto func = [i](const vector<double>& x) {
            return hillFunction(i, x);
            };

        Individual bestResult;
        bestResult.functionVal = numeric_limits<double>::max();

        int runsUsed = 0;
        int bestEvals = 0;
        double bestTime = 0;

        for (int run = 0; run < maxRuns; run++) {
            runsUsed++;

            auto start = chrono::high_resolution_clock::now();

            DifferentialEvolution de(
                NP, F, CR, M,
                lower, upper,
                func,
                knownValue,
                0.01
            );

            Individual result = de.optimize();

            auto end = chrono::high_resolution_clock::now();
            double timeMs = chrono::duration<double, milli>(end - start).count();

            if (result.functionVal < bestResult.functionVal) {
                bestResult = result;
                bestEvals = de.getEvaluations();
                bestTime = timeMs;
            }

            if (fabs(bestResult.functionVal - knownValue) <= 0.01) {
                break;
            }
        }

        bool isSuccess = fabs(bestResult.functionVal - knownValue) <= 0.01;
        double error = fabs(bestResult.functionVal - knownValue);

        totalEval += bestEvals;
        totalRuns += runsUsed;
        totalTime += bestTime;

        if (isSuccess)
            success++;
       
        ofstream outFile(filename, ios::app);
        outFile << i << ";"
            << fixed << setprecision(10) << bestResult.functionVal << ";"
            << knownValue << ";"
            << error << ";"
            << bestEvals << ";"
            << bestTime << ";"
            << runsUsed << ";"
            << (isSuccess ? "Да" : "Нет") << "\n";
        outFile.close();

        if ((i + 1) % 100 == 0)
            cout << "Обработано " << i + 1 << " задач...\n";
    }

    cout << "\n" << string(60, '-') << "\n";
    cout << "РЕЗУЛЬТАТЫ СОХРАНЕНЫ В ФАЙЛ: " << filename << "\n";
    cout << string(60, '-') << "\n";
    cout << "Успешно решено: " << success << "/1000 ("
        << (100.0 * success / 100) << "%)\n";
    cout << "Среднее время на задачу: " << totalTime / 1000 << " мс\n";
    cout << "Среднее количество вычислений: " << totalEval / 1000 << "\n";
    cout << "Среднее количество запусков: " << (double)totalRuns / 1000 << "\n";
}

// ---- Функции для вывода статистики DE ----
void printDEProgress(const DifferentialEvolution& de, int generation, int totalGenerations) {
    if (generation == 0) {
        cout << "\nПРОЦЕСС ОПТИМИЗАЦИИ DE:\n";
        cout << string(90, '-') << "\n";
        cout << setw(10) << "Поколение"
            << setw(20) << "Лучшее значение"
            << setw(20) << "Среднее значение"
            << setw(20) << "Худшее значение"
            << setw(20) << "Лучшая точка" << "\n";
        cout << string(90, '-') << "\n";
    }

    const auto& info = de.history.back();
    cout << setw(10) << info.generation
        << setw(20) << fixed << setprecision(8) << info.bestFunctionVal
        << setw(20) << info.avgFunctionVal
        << setw(20) << info.worstFunctionVal
        << setw(20);

    cout << "(";
    for (size_t i = 0; i < info.bestPoint.size() && i < 3; i++) {
        cout << fixed << setprecision(4) << info.bestPoint[i];
        if (i < info.bestPoint.size() - 1 && i < 2) cout << ", ";
    }
    if (info.bestPoint.size() > 3) cout << ", ...";
    cout << ")" << "\n";
}

void printDEHistory(const DifferentialEvolution& de) {
    cout << "\n" << string(90, '=') << "\n";
    cout << "ИСТОРИЯ ОПТИМИЗАЦИИ DIFFERENTIAL EVOLUTION\n";
    cout << string(90, '=') << "\n";

    cout << setw(10) << "Поколение"
        << setw(20) << "Лучшее значение"
        << setw(20) << "Среднее значение"
        << setw(20) << "Худшее значение"
        << setw(20) << "Лучшая точка" << "\n";
    cout << string(90, '-') << "\n";

    for (const auto& info : de.history) {
        cout << setw(10) << info.generation
            << setw(20) << fixed << setprecision(8) << info.bestFunctionVal
            << setw(20) << info.avgFunctionVal
            << setw(20) << info.worstFunctionVal
            << setw(20);

        cout << "(";
        for (size_t i = 0; i < info.bestPoint.size() && i < 3; i++) {
            cout << fixed << setprecision(4) << info.bestPoint[i];
            if (i < info.bestPoint.size() - 1 && i < 2) cout << ", ";
        }
        if (info.bestPoint.size() > 3) cout << ", ...";
        cout << ")" << "\n";
    }
    cout << string(90, '-') << "\n";
    cout << "Всего поколений: " << de.history.size() << "\n";
}

void printDEConvergence(const DifferentialEvolution& de) {
    if (de.history.empty()) return;

    cout << "\n" << string(90, '=') << "\n";
    cout << "СХОДИМОСТЬ АЛГОРИТМА\n";
    cout << string(90, '=') << "\n";

    int convergenceGen = de.history.size() - 1;
    double finalBest = de.history.back().bestFunctionVal;

    for (size_t i = 0; i < de.history.size(); i++) {
        if (de.history[i].bestFunctionVal == finalBest &&
            de.history[i].avgFunctionVal == finalBest) {
            convergenceGen = i;
            break;
        }
    }

    cout << "Поколение полной сходимости: " << convergenceGen << "\n";
    cout << "Финальное значение функции: " << fixed << setprecision(12) << finalBest << "\n";

    cout << "\nКЛЮЧЕВЫЕ ЭТАПЫ СХОДИМОСТИ:\n";
    cout << string(70, '-') << "\n";
    cout << setw(10) << "Поколение" << setw(25) << "Лучшее значение" << setw(25) << "Среднее значение" << "\n";
    cout << string(70, '-') << "\n";

    vector<int> stages = { 0, 10, 20, 50, 100, 200, 500, 1000 };
    for (int stage : stages) {
        if (stage < (int)de.history.size()) {
            cout << setw(10) << de.history[stage].generation
                << setw(25) << de.history[stage].bestFunctionVal
                << setw(25) << de.history[stage].avgFunctionVal << "\n";
        }
    }
    cout << string(70, '-') << "\n";
}

// растригин для диф эволюции  --  вывод по поколениям
void demoRastriginDEWithTable(int dimension) {
    cout << "\n" << string(60, '=') << "\n";
    cout << "ДИФФЕРЕНЦИАЛЬНАЯ ЭВОЛЮЦИЯ - ФУНКЦИЯ РАСТРИГИНА (" << dimension << "D)\n";
    cout << string(60, '=') << "\n";

    vector<double> lower, upper;
    getRastriginBounds(dimension, lower, upper);

    int NP = 50;
    double F = 0.8;
    double CR = 0.9;
    int M = 500;

    cout << "Параметры алгоритма:\n";
    cout << "  NP (размер популяции) = " << NP << "\n";
    cout << "  F (весовой коэффициент) = " << F << "\n";
    cout << "  CR (вероятность кроссовера) = " << CR << "\n";
    cout << "  M (количество поколений) = " << M << "\n\n";

    cout << "Область поиска:\n";
    for (int i = 0; i < dimension; i++) {
        cout << "  x" << i << " в [" << lower[i] << ", " << upper[i] << "]\n";
    }
    cout << "\n";

    vector<double> optimum = getRastriginOptimum(dimension);
    double optimumValue = rastrigin(optimum);

    double tolerance = 1e-12;

    DifferentialEvolution de(NP, F, CR, M, lower, upper, rastrigin, optimumValue, tolerance);

    cout << "Выполняется оптимизация...\n";
    Individual result = de.optimize();

    printDEHistory(de);

    cout << "\nРЕЗУЛЬТАТ ПОИСКА (DE):\n";
    printDEResult(result);

    cout << "\nИЗВЕСТНЫЙ ГЛОБАЛЬНЫЙ МИНИМУМ:\n";
    cout << "Точка: (";
    for (size_t i = 0; i < optimum.size(); i++) {
        cout << optimum[i];
        if (i < optimum.size() - 1) cout << ", ";
    }
    cout << "), Значение: " << optimumValue << "\n";

    cout << "\nОшибка: " << fabs(result.functionVal - optimumValue) << "\n";
    cout << "Вычислений функции: " << de.getEvaluations() << "\n";
}

// хилл для диф эволюции  --  вывод по поколениям
void demoHillDEWithTable(int problemIndex) {
    cout << "\n" << string(60, '=') << "\n";
    cout << "ДИФФЕРЕНЦИАЛЬНАЯ ЭВОЛЮЦИЯ - ФУНКЦИЯ ХИЛЛА (задача #" << problemIndex << ")\n";
    cout << string(60, '=') << "\n";

    vector<double> lower = { 0.0 };
    vector<double> upper = { 1.0 };

    int NP = 30;
    double F = 0.7;
    double CR = 0.8;
    int M = 200;

    cout << "Параметры алгоритма:\n";
    cout << "  NP (размер популяции) = " << NP << "\n";
    cout << "  F (весовой коэффициент) = " << F << "\n";
    cout << "  CR (вероятность кроссовера) = " << CR << "\n";
    cout << "  M (количество поколений) = " << M << "\n\n";

    cout << "Область поиска: x в [" << lower[0] << ", " << upper[0] << "]\n\n";

    double knownOptimumValue;
    vector<double> knownOptimum = getHillOptimum(problemIndex, knownOptimumValue);

    cout << "Известный минимум: f(" << knownOptimum[0] << ") = " << knownOptimumValue << "\n\n";

    auto hillFunc = [problemIndex](const vector<double>& x) -> double {
        return hillFunction(problemIndex, x);
        };

    cout << "Выполняется оптимизация...\n";
    DifferentialEvolution de(NP, F, CR, M, lower, upper, hillFunc, knownOptimumValue, 0.01);
    Individual result = de.optimize();

    printDEHistory(de);

    cout << "\nРЕЗУЛЬТАТ ПОИСКА (DE):\n";
    printDEResult(result);

    cout << "\nРАЗНОСТЬ С ИЗВЕСТНЫМ МИНИМУМОМ:\n";
    cout << "Абсолютная ошибка: " << fabs(result.functionVal - knownOptimumValue) << "\n";
    cout << "Ошибка по x: " << fabs(result.genes[0] - knownOptimum[0]) << "\n";
    cout << "Вычислений функции: " << de.getEvaluations() << "\n";
}

void printDECompactHistory(const DifferentialEvolution& de) {
    cout << "\n" << string(80, '-') << "\n";
    cout << "СХОДИМОСТЬ ПО ПОКОЛЕНИЯМ (лучшее значение):\n";
    cout << string(80, '-') << "\n";
    cout << setw(12) << "Поколение" << setw(20) << "Лучшее значение" << setw(25) << "Среднее значение" << "\n";
    cout << string(80, '-') << "\n";

    for (const auto& info : de.history) {
        if (info.generation % 20 == 0 || info.generation == de.history.size() - 1) {
            cout << setw(12) << info.generation
                << setw(20) << fixed << setprecision(8) << info.bestFunctionVal
                << setw(25) << info.avgFunctionVal << "\n";
        }
    }
    cout << string(80, '-') << "\n";
}

//  Шекель  //

// шекель с прямым перебором
void demoShekelDirect(int problemIndex) {
    cout << "\nФУНКЦИЯ ШЕКЕЛЯ #" << problemIndex << "\n";

    vector<double> lower = { 0.0 };
    vector<double> upper = { 10.0 };
    double step = 0.01;

    double knownValue;
    vector<double> optimum = getShekelOptimum(problemIndex, knownValue);

    auto func = [problemIndex](const vector<double>& x) {
        return shekelFunction(problemIndex, x);
        };

    Point result = runDirectSearch(lower, upper, step, func);

    printResult(result);

    cout << "Известный минимум: f(" << optimum[0] << ") = " << knownValue << "\n";
    cout << "Ошибка: " << fabs(result.value - knownValue) << "\n";
}

// шекель с дифф эволюцией
void demoShekelDE(int problemIndex) {
    cout << "\nDE - ФУНКЦИЯ ШЕКЕЛЯ #" << problemIndex << "\n";

    vector<double> lower = { 0.0 };
    vector<double> upper = { 10.0 };

    double knownValue;
    vector<double> optimum = getShekelOptimum(problemIndex, knownValue);

    auto func = [problemIndex](const vector<double>& x) {
        return shekelFunction(problemIndex, x);
        };

    int NP = 50;
    double F = 0.8;
    double CR = 0.9;
    int M = 500;

    Individual result = runDE(
        lower, upper,
        NP, F, CR, M,
        func,
        knownValue
    );

    printDEResult(result);

    cout << "Известный минимум: f(" << optimum[0] << ") = " << knownValue << "\n";
    cout << "Ошибка: " << fabs(result.functionVal - knownValue) << "\n";
    cout << "Ошибка по x: " << fabs(result.genes[0] - optimum[0]) << "\n";
    cout << "Вычислений функции: " << result.evaluationsCount << "\n";
}

// весь шекель для прямого перебора
void testShekelDirectAll() {
    cout << "\nТЕСТ " << NUM_SHEKEL_TESTS << " ЗАДАЧ ШЕКЕЛЯ (ПРЯМОЙ ПЕРЕБОР)\n";

    vector<double> lower = { 0.0 };
    vector<double> upper = { 10.0 };
    double step = 0.01;

    int success = 0;
    long long totalEval = 0;
    double totalTime = 0.0;

    for (int i = 0; i < NUM_SHEKEL_TESTS; i++) {
        double knownValue;
        getShekelOptimum(i, knownValue);

        auto func = [i](const vector<double>& x) {
            return shekelFunction(i, x);
            };

        auto start = chrono::high_resolution_clock::now();

        Point result = runDirectSearch(lower, upper, step, func);

        auto end = chrono::high_resolution_clock::now();
        double timeMs = chrono::duration<double, milli>(end - start).count();

        totalTime += timeMs;
        totalEval += 1001;

        if (fabs(result.value - knownValue) <= 0.01)
            success++;

        if ((i + 1) % 100 == 0)
            cout << "Обработано " << i + 1 << " задач...\n";
    }

    double avgTime = totalTime / NUM_SHEKEL_TESTS;
    double avgEval = static_cast<double>(totalEval) / NUM_SHEKEL_TESTS;
    double successRate = 100.0 * success / NUM_SHEKEL_TESTS;

    cout << "\n" << string(60, '-') << "\n";
    cout << "РЕЗУЛЬТАТЫ ТЕСТИРОВАНИЯ ПРЯМОГО ПЕРЕБОРА (ШЕКЕЛЬ):\n";
    cout << string(60, '-') << "\n";
    cout << fixed << setprecision(6);
    cout << "Шаг сетки: " << step << "\n";
    cout << "Точек сетки на задачу: " << 1001 << "\n";
    cout << "Успешно решено: " << success << "/" << NUM_SHEKEL_TESTS
        << " (" << successRate << "%)\n";
    cout << "Среднее время на задачу: " << avgTime << " мс\n";
    cout << "Среднее количество вычислений на задачу: " << (int)avgEval << "\n";
}

// весь шекель с дифф эволюцией
void testShekelDEAll() {
    cout << "\nТЕСТ " << NUM_SHEKEL_TESTS << " ЗАДАЧ ШЕКЕЛЯ (DE, МНОГОКРАТНЫЕ ЗАПУСКИ)\n";

    vector<double> lower = { 0.0 };
    vector<double> upper = { 10.0 };

    int NP = 50;
    double F = 0.8;
    double CR = 0.9;
    int M = 500;
    int maxRuns = 5;
    double tolerance = 0.01;

    int success = 0;
    long long totalEval = 0;
    double totalTime = 0.0;
    int totalRuns = 0;
    double totalError = 0.0;

    auto startTotal = chrono::high_resolution_clock::now();

    for (int i = 0; i < NUM_SHEKEL_TESTS; i++) {
        double knownValue;
        vector<double> optimum = getShekelOptimum(i, knownValue);

        auto func = [i](const vector<double>& x) {
            return shekelFunction(i, x);
            };

        Individual bestResult;
        bestResult.functionVal = numeric_limits<double>::max();

        int runsUsed = 0;
        int bestEvals = 0;

        for (int run = 0; run < maxRuns; run++) {
            runsUsed++;

            DifferentialEvolution de(
                NP, F, CR, M,
                lower, upper,
                func,
                knownValue,
                tolerance
            );

            Individual result = de.optimize();

            if (result.functionVal < bestResult.functionVal) {
                bestResult = result;
                bestEvals = de.getEvaluations();
            }

            if (fabs(bestResult.functionVal - knownValue) <= tolerance) {
                break;
            }
        }

        double error = fabs(bestResult.functionVal - knownValue);
        totalError += error;
        totalEval += bestEvals;
        totalRuns += runsUsed;

        if (error <= tolerance)
            success++;

        if ((i + 1) % 100 == 0)
            cout << "Обработано " << i + 1 << " задач...\n";
    }

    auto endTotal = chrono::high_resolution_clock::now();
    double totalTimeMs = chrono::duration<double, milli>(endTotal - startTotal).count();

    cout << "\n" << string(60, '-') << "\n";
    cout << "РЕЗУЛЬТАТЫ ТЕСТИРОВАНИЯ DE (ШЕКЕЛЬ):\n";
    cout << string(60, '-') << "\n";
    cout << fixed << setprecision(6);
    cout << "Параметры: NP=" << NP << ", F=" << F << ", CR=" << CR << ", M=" << M << "\n";
    cout << "Максимум запусков: " << maxRuns << "\n";
    cout << "Успешно решено: " << success << "/" << NUM_SHEKEL_TESTS
        << " (" << (100.0 * success / NUM_SHEKEL_TESTS) << "%)\n";
    cout << "Средняя ошибка: " << totalError / NUM_SHEKEL_TESTS << "\n";
    cout << "Среднее время на задачу: " << totalTimeMs / NUM_SHEKEL_TESTS << " мс\n";
    cout << "Среднее количество вычислений на задачу: " << totalEval / NUM_SHEKEL_TESTS << "\n";
    cout << "Среднее количество запусков на задачу: " << (double)totalRuns / NUM_SHEKEL_TESTS << "\n";
}

//  Гришагин  //

// гришагин для прямого перебора
void demoGrishaginDirect(int problemIndex) {
    cout << "\nФУНКЦИЯ ГРИШАГИНА #" << problemIndex << "\n";

    vector<double> lower, upper;
    getGrishaginBounds(problemIndex, lower, upper);

    double step = 0.01;

    double knownValue;
    vector<double> optimum = getGrishaginOptimum(problemIndex, knownValue);

    auto func = [problemIndex](const vector<double>& x) {
        return grishaginFunction(problemIndex, x);
        };

    Point result = runDirectSearch(lower, upper, step, func);

    printResult(result);

    cout << "Известный минимум: f(" << optimum[0] << ", " << optimum[1] << ") = " << knownValue << "\n";
    cout << "Ошибка: " << fabs(result.value - knownValue) << "\n";
    cout << "Ошибка по x: " << sqrt(pow(result.point[0] - optimum[0], 2) + pow(result.point[1] - optimum[1], 2)) << "\n";
}

// гришагин для дифф эволюции
void demoGrishaginDE(int problemIndex) {
    cout << "\nDE - ФУНКЦИЯ ГРИШАГИНА #" << problemIndex << "\n";

    vector<double> lower, upper;
    getGrishaginBounds(problemIndex, lower, upper);

    double knownValue;
    vector<double> optimum = getGrishaginOptimum(problemIndex, knownValue);

    auto func = [problemIndex](const vector<double>& x) {
        return grishaginFunction(problemIndex, x);
        };

    int NP = 80;
    double F = 0.8;
    double CR = 0.9;
    int M = 500;

    Individual result = runDE(
        lower, upper,
        NP, F, CR, M,
        func,
        knownValue
    );

    printDEResult(result);

    cout << "Известный минимум: f(" << optimum[0] << ", " << optimum[1] << ") = " << knownValue << "\n";
    cout << "Ошибка: " << fabs(result.functionVal - knownValue) << "\n";
    cout << "Ошибка по x: " << sqrt(pow(result.genes[0] - optimum[0], 2) + pow(result.genes[1] - optimum[1], 2)) << "\n";
}

// весь гришагин для прямого перебора
void testGrishaginDirectAll() {
    cout << "\nТЕСТ " << NUM_GRISHAGIN_TESTS << " ЗАДАЧ ГРИШАГИНА (ПРЯМОЙ ПЕРЕБОР)\n";

    vector<double> lower = { 0.0, 0.0 };
    vector<double> upper = { 1.0, 1.0 };
    double step = 0.009;

    int success = 0;
    long long totalEval = 0;
    double totalTime = 0.0;

    int pointsPerTask = (int)((upper[0] - lower[0]) / step + 1) *
        (int)((upper[1] - lower[1]) / step + 1);

    for (int i = 1; i <= NUM_GRISHAGIN_TESTS; i++) {
        double knownValue;
        vector<double> optimum = getGrishaginOptimum(i, knownValue);

        auto func = [i](const vector<double>& x) {
            return grishaginFunction(i, x);
            };

        auto start = chrono::high_resolution_clock::now();

        Point result = runDirectSearch(lower, upper, step, func);

        auto end = chrono::high_resolution_clock::now();
        double timeMs = chrono::duration<double, milli>(end - start).count();

        totalTime += timeMs;
        totalEval += pointsPerTask;

        double distance = sqrt(pow(result.point[0] - optimum[0], 2) +
            pow(result.point[1] - optimum[1], 2));

        double tolerance = step * 1.5;

        if (distance <= tolerance)
            success++;

        if (i % 10 == 0)
            cout << "Обработано " << i << " задач...\n";
    }

    double avgTime = totalTime / NUM_GRISHAGIN_TESTS;
    double avgEval = static_cast<double>(totalEval) / NUM_GRISHAGIN_TESTS;
    double successRate = 100.0 * success / NUM_GRISHAGIN_TESTS;

    cout << "\n" << string(60, '-') << "\n";
    cout << "РЕЗУЛЬТАТЫ ТЕСТИРОВАНИЯ ПРЯМОГО ПЕРЕБОРА (ГРИШАГИН):\n";
    cout << string(60, '-') << "\n";
    cout << fixed << setprecision(6);
    cout << "Шаг сетки: " << step << "\n";
    cout << "Точек сетки на задачу: " << pointsPerTask << "\n";
    cout << "Успешно решено: " << success << "/" << NUM_GRISHAGIN_TESTS
        << " (" << successRate << "%)\n";
    cout << "Среднее время на задачу: " << avgTime << " мс\n";
    cout << "Среднее количество вычислений на задачу: " << (int)avgEval << "\n";
}

// весь гришагин для дифф эволюции
void testGrishaginDEAll() {
    cout << "\nТЕСТ " << NUM_GRISHAGIN_TESTS << " ЗАДАЧ ГРИШАГИНА (DE)\n";

    vector<double> lower = { 0.0, 0.0 };
    vector<double> upper = { 1.0, 1.0 };
    
    int NP = 120;
    double F = 0.95;
    double CR = 0.70;
    int M = 700;
    int maxRuns = 10;
    double tolerance = 0.01;

    int success = 0;
    long long totalEval = 0;
    double totalTime = 0.0;
    int totalRuns = 0;
    double totalCoordError = 0.0;

    auto startTotal = chrono::high_resolution_clock::now();

    for (int i = 1; i <= NUM_GRISHAGIN_TESTS; i++) {
        double knownValue;
        vector<double> optimum = getGrishaginOptimum(i, knownValue);

        auto func = [i](const vector<double>& x) {
            return grishaginFunction(i, x);
            };

        Individual bestResult;
        bestResult.functionVal = numeric_limits<double>::max();

        int runsUsed = 0;
        int bestEvals = 0;

        for (int run = 0; run < maxRuns; run++) {
            runsUsed++;

            auto start = chrono::high_resolution_clock::now();

            DifferentialEvolution de(
                NP, F, CR, M,
                lower, upper,
                func,
                knownValue,
                0.01
            );

            Individual result = de.optimize();

            auto end = chrono::high_resolution_clock::now();
            double timeMs = chrono::duration<double, milli>(end - start).count();

            if (result.functionVal < bestResult.functionVal) {
                bestResult = result;
                bestEvals = de.getEvaluations();
            }

            double coordError = sqrt(pow(result.genes[0] - optimum[0], 2) +
                pow(result.genes[1] - optimum[1], 2));
            if (coordError <= tolerance) {
                break;
            }
        }

        double coordError = sqrt(pow(bestResult.genes[0] - optimum[0], 2) +
            pow(bestResult.genes[1] - optimum[1], 2));

        totalCoordError += coordError;
        totalEval += bestEvals;
        totalRuns += runsUsed;

        if (coordError <= tolerance)
            success++;

        if (i % 10 == 0)
            cout << "Обработано " << i << " задач...\n";
    }

    auto endTotal = chrono::high_resolution_clock::now();
    double totalTimeMs = chrono::duration<double, milli>(endTotal - startTotal).count();

    cout << "\n" << string(60, '-') << "\n";
    cout << "РЕЗУЛЬТАТЫ ТЕСТИРОВАНИЯ DE (ГРИШАГИН):\n";
    cout << string(60, '-') << "\n";
    cout << fixed << setprecision(6);
    cout << "Параметры: NP=" << NP << ", F=" << F << ", CR=" << CR << ", M=" << M << "\n";
    cout << "Максимум запусков: " << maxRuns << "\n";
    cout << "Успешно решено: " << success << "/" << NUM_GRISHAGIN_TESTS
        << " (" << (100.0 * success / NUM_GRISHAGIN_TESTS) << "%)\n";
    cout << "Среднее время на задачу: " << totalTimeMs / NUM_GRISHAGIN_TESTS << " мс\n";
    cout << "Среднее количество вычислений: " << totalEval / NUM_GRISHAGIN_TESTS << "\n";
    cout << "Среднее количество запусков: " << (double)totalRuns / NUM_GRISHAGIN_TESTS << "\n";
}

void testGrishaginDEAllWithSaving() {
    cout << "\nТЕСТ " << NUM_GRISHAGIN_TESTS << " ЗАДАЧ ГРИШАГИНА (DE) С СОХРАНЕНИЕМ РЕЗУЛЬТАТОВ\n";

    vector<double> lower = { 0.0, 0.0 };
    vector<double> upper = { 1.0, 1.0 };

    int NP = 90;
    double F = 0.85;
    double CR = 0.95;
    int M = 600;
    int maxRuns = 6;
    double tolerance = 0.01;

    string filename = "grishagin_results_de.csv";
    ofstream outFile(filename);
    outFile << "Номер задачи;Найденное значение;Известное значение;Ошибка;"
        << "Вычислений;Время(мс);Количество запусков;Успех\n";
    outFile.close();

    int success = 0;
    long long totalEval = 0;
    double totalTime = 0.0;
    int totalRuns = 0;
    double totalError = 0.0;

    auto startTotal = chrono::high_resolution_clock::now();

    for (int i = 1; i <= NUM_GRISHAGIN_TESTS; i++) {
        double knownValue;
        vector<double> optimum = getGrishaginOptimum(i, knownValue);

        auto func = [i](const vector<double>& x) {
            return grishaginFunction(i, x);
            };

        Individual bestResult;
        bestResult.functionVal = numeric_limits<double>::max();

        int runsUsed = 0;
        int bestEvals = 0;
        double bestTime = 0;

        for (int run = 0; run < maxRuns; run++) {
            runsUsed++;

            auto start = chrono::high_resolution_clock::now();

            DifferentialEvolution de(
                NP, F, CR, M,
                lower, upper,
                func,
                knownValue,
                tolerance
            );

            Individual result = de.optimize();

            auto end = chrono::high_resolution_clock::now();
            double timeMs = chrono::duration<double, milli>(end - start).count();

            if (result.functionVal < bestResult.functionVal) {
                bestResult = result;
                bestEvals = de.getEvaluations();
                bestTime = timeMs;
            }

            if (fabs(bestResult.functionVal - knownValue) <= tolerance) {
                break;
            }
        }

        double error = fabs(bestResult.functionVal - knownValue);
        bool isSuccess = (error <= tolerance);

        totalError += error;
        totalEval += bestEvals;
        totalRuns += runsUsed;
        totalTime += bestTime;

        if (isSuccess)
            success++;

        ofstream outFile(filename, ios::app);
        outFile << i << ";"
            << fixed << setprecision(10) << bestResult.functionVal << ";"
            << knownValue << ";"
            << error << ";"
            << bestEvals << ";"
            << bestTime << ";"
            << runsUsed << ";"
            << (isSuccess ? "Да" : "Нет") << "\n";
        outFile.close();

        if (i % 10 == 0)
            cout << "Обработано " << i << " задач...\n";
    }

    auto endTotal = chrono::high_resolution_clock::now();
    double totalTimeMs = chrono::duration<double, milli>(endTotal - startTotal).count();

    cout << "\n" << string(60, '-') << "\n";
    cout << "РЕЗУЛЬТАТЫ СОХРАНЕНЫ В ФАЙЛ: " << filename << "\n";
    cout << string(60, '-') << "\n";
    cout << fixed << setprecision(6);
    cout << "Параметры: NP=" << NP << ", F=" << F << ", CR=" << CR << ", M=" << M << "\n";
    cout << "Максимум запусков: " << maxRuns << "\n";
    cout << "Успешно решено: " << success << "/" << NUM_GRISHAGIN_TESTS
        << " (" << (100.0 * success / NUM_GRISHAGIN_TESTS) << "%)\n";
    cout << "Средняя ошибка: " << totalError / NUM_GRISHAGIN_TESTS << "\n";
    cout << "Среднее время на задачу: " << totalTimeMs / NUM_GRISHAGIN_TESTS << " мс\n";
    cout << "Среднее количество вычислений: " << totalEval / NUM_GRISHAGIN_TESTS << "\n";
    cout << "Среднее количество запусков: " << (double)totalRuns / NUM_GRISHAGIN_TESTS << "\n";
}

//  GKLS  //

void demoGKLSDE(int problemIndex, int dim, GKLSClass type, GKLSFuncionType funcType) {
    string typeName = (type == Simple) ? "Simple" : "Hard";
    string funcTypeName;
    switch (funcType) {
        case TND: funcTypeName = "ND (non-differentiable)"; break;
        case TD: funcTypeName = "D (differentiable)"; break;
        case TD2: funcTypeName = "D2 (twice differentiable)"; break;
    }

    cout << "\nDE - ФУНКЦИЯ GKLS #" << problemIndex << " (" << dim << "D, " << typeName << ", " << funcTypeName << ")\n";

    vector<double> lower, upper;
    getGKLSBounds(dim, lower, upper);

    GKLSGlobalMinimaInfo minimaInfo = getGKLSGlobalMinima(problemIndex, dim, type, funcType);
    double knownValue = minimaInfo.value;

    auto func = [problemIndex, dim, type, funcType](const vector<double>& x) {
        return gklsFunction(problemIndex, x, dim, type, funcType);
        };

    int NP = 100;
    double F = 0.8;
    double CR = 0.9;
    int M = 1000;

    Individual result = runDE(lower, upper, NP, F, CR, M, func, knownValue);

    printDEResult(result);

    cout << "Известный минимум: " << knownValue << "\n";
    cout << "Ошибка по значению: " << fabs(result.functionVal - knownValue) << "\n";
}

struct GKLSStats {
    int totalProblems;
    int totalSuccess;
    long long totalEval;
    double totalTime;
    int totalRuns;
    vector<tuple<int, GKLSClass, GKLSFuncionType, int, double>> failedTasks;
};

GKLSStats runGKLSHardTNDTestForDimension(int dim, double step) {
    GKLSStats stats;
    stats.totalProblems = 0;
    stats.totalSuccess = 0;
    stats.totalEval = 0;
    stats.totalTime = 0.0;

    GKLSClass gklsClass = Hard;
    GKLSFuncionType funcType = TND;

    for (int problemIdx = 1; problemIdx <= NUM_GKLS_TESTS; problemIdx++) {
        stats.totalProblems++;

        vector<double> lower, upper;
        getGKLSBounds(dim, lower, upper);

        GKLSGlobalMinimaInfo minimaInfo = getGKLSGlobalMinima(problemIdx, dim, gklsClass, funcType);
        double knownValue = minimaInfo.value;
        vector<vector<double>> knownPoints = minimaInfo.points;

        auto func = [problemIdx, dim, gklsClass, funcType](const vector<double>& x) {
            return gklsFunction(problemIdx, x, dim, gklsClass, funcType);
            };

        auto start = chrono::high_resolution_clock::now();
        Point result = runDirectSearch(lower, upper, step, func);
        auto end = chrono::high_resolution_clock::now();

        double timeMs = chrono::duration<double, milli>(end - start).count();
        stats.totalTime += timeMs;

        int pointsPerProblem = 1;
        for (int d = 0; d < dim; d++) {
            pointsPerProblem *= static_cast<int>((upper[d] - lower[d]) / step) + 1;
        }
        stats.totalEval += pointsPerProblem;

        bool success = false;
        for (const auto& knownPoint : knownPoints) {
            double dist = 0.0;
            for (int d = 0; d < dim; d++) {
                dist += (result.point[d] - knownPoint[d]) * (result.point[d] - knownPoint[d]);
            }
            dist = sqrt(dist);

            double threshold = step * sqrt(dim) / 2.0;
            if (dist <= threshold) {
                success = true;
                break;
            }
        }

        if (success) {
            stats.totalSuccess++;
        }
        else {
            double minDist = 1e100;
            for (const auto& knownPoint : knownPoints) {
                double dist = 0.0;
                for (int d = 0; d < dim; d++) {
                    dist += (result.point[d] - knownPoint[d]) * (result.point[d] - knownPoint[d]);
                }
                dist = sqrt(dist);
                if (dist < minDist) minDist = dist;
            }
            stats.failedTasks.push_back(make_tuple(dim, gklsClass, funcType, problemIdx, minDist));
        }

        if (stats.totalProblems % 1 == 0) {
            cout << "  Обработано " << stats.totalProblems << " задач...\n";
        }
    }

    return stats;
}

GKLSStats runGKLSDeTestForDimension(int dim, int NP, double F, double CR, int M, int maxRuns) {
    GKLSStats stats;
    stats.totalProblems = 0;
    stats.totalSuccess = 0;
    stats.totalEval = 0;
    stats.totalTime = 0.0;
    stats.totalRuns = 0;

    GKLSClass gklsClass = Hard;
    GKLSFuncionType funcType = TND;

    for (int problemIdx = 1; problemIdx <= NUM_GKLS_TESTS; problemIdx++) {
        stats.totalProblems++;

        vector<double> lower, upper;
        getGKLSBounds(dim, lower, upper);

        double knownValue = getGKLSMinimumValue(problemIdx, dim, gklsClass, funcType);

        auto func = [problemIdx, dim, gklsClass, funcType](const vector<double>& x) {
            return gklsFunction(problemIdx, x, dim, gklsClass, funcType);
            };

        auto start = chrono::high_resolution_clock::now();

        Individual bestResult;
        bestResult.functionVal = numeric_limits<double>::max();

        int bestEvals = 0;
        int runsUsed = 0;

        for (int run = 0; run < maxRuns; run++) {
            runsUsed++;

            DifferentialEvolution de(NP, F, CR, M, lower, upper, func, knownValue, 0.01);
            Individual result = de.optimize();

            if (result.functionVal < bestResult.functionVal) {
                bestResult = result;
                bestEvals = de.getEvaluations();
            }

            if (fabs(bestResult.functionVal - knownValue) <= 0.01) {
                break;
            }
        }

        auto end = chrono::high_resolution_clock::now();
        double timeMs = chrono::duration<double, milli>(end - start).count();
        stats.totalTime += timeMs;
        stats.totalEval += bestEvals;
        stats.totalRuns += runsUsed;

        double error = fabs(bestResult.functionVal - knownValue);
        if (error <= 0.01) {
            stats.totalSuccess++;
        }
        else {
            stats.failedTasks.push_back(make_tuple(dim, gklsClass, funcType, problemIdx, error));
        }

        if (stats.totalProblems % 10 == 0) {
            cout << "  Обработано " << stats.totalProblems << " задач...\n";
        }
    }

    return stats;
}

void printGKLSStats(const GKLSStats& stats, int dim, int totalTasks) {
    cout << "\n" << string(60, '-') << "\n";
    cout << "РЕЗУЛЬТАТЫ ДЛЯ РАЗМЕРНОСТИ " << dim << "D (Hard, TND):\n";
    cout << string(60, '-') << "\n";
    cout << "  Всего задач: " << stats.totalProblems << "\n";
    cout << "  Успешно решено: " << stats.totalSuccess << "/" << stats.totalProblems
        << " (" << (100.0 * stats.totalSuccess / stats.totalProblems) << "%)\n";

    double avgRuns = (double)stats.totalRuns / stats.totalProblems;
    cout << "  Среднее число запусков на задачу: " << fixed << setprecision(2) << avgRuns << "\n";

    cout << "  Среднее время на задачу: " << fixed << setprecision(3)
        << (stats.totalTime / stats.totalProblems) << " мс\n";
    cout << "  Среднее количество вычислений на задачу: " << stats.totalEval / stats.totalProblems << "\n";

    if (!stats.failedTasks.empty()) {
        cout << "  Неудачных задач: " << stats.failedTasks.size() << "\n";
        cout << "  Ошибки:\n";
        for (const auto& task : stats.failedTasks) {
            cout << "    #" << get<3>(task) << ": ошибка=" << get<4>(task) << "\n";
        }
    }
}

void testGKLSDirect2D() {
    cout << "\n" << string(60, '=') << "\n";
    cout << "ТЕСТИРОВАНИЕ ПРЯМОГО ПЕРЕБОРА НА GKLS (2D, HARD, TND)\n";
    cout << string(60, '=') << "\n";

    int dim = 2;
    double step = 0.01;

    cout << "Шаг сетки: " << step << "\n";

    auto startTotal = chrono::high_resolution_clock::now();
    GKLSStats stats = runGKLSHardTNDTestForDimension(dim, step);
    auto endTotal = chrono::high_resolution_clock::now();
    double totalTimeMs = chrono::duration<double, milli>(endTotal - startTotal).count();

    printGKLSStats(stats, dim, step);
    cout << "  Общее время: " << totalTimeMs << " мс (" << totalTimeMs / 1000 << " с)\n";
}

void testGKLSDirect3D() {
    cout << "\n" << string(60, '=') << "\n";
    cout << "ТЕСТИРОВАНИЕ ПРЯМОГО ПЕРЕБОРА НА GKLS (3D, HARD, TND)\n";
    cout << string(60, '=') << "\n";

    int dim = 3;
    double step = 0.01;

    cout << "Шаг сетки: " << step << "\n";

    auto startTotal = chrono::high_resolution_clock::now();
    GKLSStats stats = runGKLSHardTNDTestForDimension(dim, step);
    auto endTotal = chrono::high_resolution_clock::now();
    double totalTimeMs = chrono::duration<double, milli>(endTotal - startTotal).count();

    printGKLSStats(stats, dim, step);
    cout << "  Общее время: " << totalTimeMs << " мс (" << totalTimeMs / 1000 << " с)\n";
}

void testGKLSDirect4D() {
    cout << "\n" << string(60, '=') << "\n";
    cout << "ТЕСТИРОВАНИЕ ПРЯМОГО ПЕРЕБОРА НА GKLS (4D, HARD, TND)\n";
    cout << string(60, '=') << "\n";

    int dim = 4;
    double step = 0.01;

    cout << "Шаг сетки: " << step << "\n";

    auto startTotal = chrono::high_resolution_clock::now();
    GKLSStats stats = runGKLSHardTNDTestForDimension(dim, step);
    auto endTotal = chrono::high_resolution_clock::now();
    double totalTimeMs = chrono::duration<double, milli>(endTotal - startTotal).count();

    printGKLSStats(stats, dim, step);
    cout << "  Общее время: " << totalTimeMs << " мс (" << totalTimeMs / 1000 << " с)\n";
}

void testGKLSDirect5D() {
    cout << "\n" << string(60, '=') << "\n";
    cout << "ТЕСТИРОВАНИЕ ПРЯМОГО ПЕРЕБОРА НА GKLS (5D, HARD, TND)\n";
    cout << string(60, '=') << "\n";

    int dim = 5;
    double step = 0.00001;

    cout << "Шаг сетки: " << step << "\n";

    auto startTotal = chrono::high_resolution_clock::now();
    GKLSStats stats = runGKLSHardTNDTestForDimension(dim, step);
    auto endTotal = chrono::high_resolution_clock::now();
    double totalTimeMs = chrono::duration<double, milli>(endTotal - startTotal).count();

    printGKLSStats(stats, dim, step);
    cout << "  Общее время: " << totalTimeMs << " мс (" << totalTimeMs / 1000 << " с)\n";
}

void testGKLSDE2D() {
    cout << "\n" << string(60, '=') << "\n";
    cout << "ТЕСТИРОВАНИЕ DE НА GKLS (2D, HARD, TND)\n";
    cout << string(60, '=') << "\n";

    int dim = 2;
    int NP = 200;
    double F = 0.95;
    double CR = 0.90;
    int M = 800;
    int maxRuns = 10;

    cout << "Параметры DE:\n";
    cout << "NP = " << NP << "\n";
    cout << "F = " << F << "\n";
    cout << "CR = " << CR << "\n";
    cout << "M = " << M << "\n";
    cout << "maxRuns = " << maxRuns << "\n\n";

    auto startTotal = chrono::high_resolution_clock::now();
    GKLSStats stats = runGKLSDeTestForDimension(dim, NP, F, CR, M, maxRuns);
    auto endTotal = chrono::high_resolution_clock::now();
    double totalTimeMs = chrono::duration<double, milli>(endTotal - startTotal).count();

    printGKLSStats(stats, dim, 0);
    cout << "  Общее время: " << totalTimeMs << " мс (" << totalTimeMs / 1000 << " с)\n";
}

void testGKLSDE3D() {
    cout << "\n" << string(60, '=') << "\n";
    cout << "ТЕСТИРОВАНИЕ DE НА GKLS (3D, HARD, TND)\n";
    cout << string(60, '=') << "\n";

    int dim = 3;
    int NP = 600;
    double F = 0.65;
    double CR = 0.90;
    int M = 2600;
    int maxRuns = 15;

    cout << "Параметры DE:\n";
    cout << "NP = " << NP << "\n";
    cout << "F = " << F << "\n";
    cout << "CR = " << CR << "\n";
    cout << "M = " << M << "\n";
    cout << "maxRuns = " << maxRuns << "\n\n";

    auto startTotal = chrono::high_resolution_clock::now();
    GKLSStats stats = runGKLSDeTestForDimension(dim, NP, F, CR, M, maxRuns);
    auto endTotal = chrono::high_resolution_clock::now();
    double totalTimeMs = chrono::duration<double, milli>(endTotal - startTotal).count();

    printGKLSStats(stats, dim, 0);
    cout << "  Общее время: " << totalTimeMs << " мс (" << totalTimeMs / 1000 << " с)\n";
}

void testGKLSDE4D() {
    cout << "\n" << string(60, '=') << "\n";
    cout << "ТЕСТИРОВАНИЕ DE НА GKLS (4D, HARD, TND)\n";
    cout << string(60, '=') << "\n";

    int dim = 4;
    int NP = 400;
    double F = 0.70;
    double CR = 0.92;
    int M = 3000;
    int maxRuns = 15;

    cout << "Параметры DE:\n";
    cout << "NP = " << NP << "\n";
    cout << "F = " << F << "\n";
    cout << "CR = " << CR << "\n";
    cout << "M = " << M << "\n";
    cout << "maxRuns = " << maxRuns << "\n\n";

    auto startTotal = chrono::high_resolution_clock::now();
    GKLSStats stats = runGKLSDeTestForDimension(dim, NP, F, CR, M, maxRuns);
    auto endTotal = chrono::high_resolution_clock::now();
    double totalTimeMs = chrono::duration<double, milli>(endTotal - startTotal).count();

    printGKLSStats(stats, dim, 0);
    cout << "  Общее время: " << totalTimeMs << " мс (" << totalTimeMs / 1000 << " с)\n";
}

void testGKLSDE5D() {
    cout << "\n" << string(60, '=') << "\n";
    cout << "ТЕСТИРОВАНИЕ DE НА GKLS (5D, HARD, TND)\n";
    cout << string(60, '=') << "\n";

    int dim = 5;
    int NP = 800;
    double F = 0.7;
    double CR = 0.92;
    int M = 5000;
    int maxRuns = 15;

    cout << "Параметры DE:\n";
    cout << "NP = " << NP << "\n";
    cout << "F = " << F << "\n";
    cout << "CR = " << CR << "\n";
    cout << "M = " << M << "\n";
    cout << "maxRuns = " << maxRuns << "\n\n";

    auto startTotal = chrono::high_resolution_clock::now();
    GKLSStats stats = runGKLSDeTestForDimension(dim, NP, F, CR, M, maxRuns);
    auto endTotal = chrono::high_resolution_clock::now();
    double totalTimeMs = chrono::duration<double, milli>(endTotal - startTotal).count();

    printGKLSStats(stats, dim, 0);
    cout << "  Общее время: " << totalTimeMs << " мс (" << totalTimeMs / 1000 << " с)\n";
}

int main() {

    setlocale(LC_ALL, "Russian");

    int choice;

    do {
        cout << "\nМЕНЮ ТЕСТИРОВАНИЯ:\n";
        cout << "\n";
        cout << "1. Прямой перебор - Растригина (2D)\n";
        cout << "2. Прямой перебор - Растригина (3D)\n";
        cout << "3. Прямой перебор - Хилла (задача 0)\n";
        cout << "4. Прямой перебор - Хилла (задача 42)\n";
        cout << "5. Прямой перебор - Шекель (задача 0)\n";
        cout << "6. Прямой перебор - Гришагин (задача 1)\n";
        cout << "\n";
        cout << "7. Дифференциальная эволюция - Растригина (2D)\n";
        cout << "8. Дифференциальная эволюция - Растригина (3D)\n";
        cout << "9. Дифференциальная эволюция - Хилла (задача 0)\n";
        cout << "10. Дифференциальная эволюция - Хилла (задача 42)\n";
        cout << "11. Дифференциальная эволюция - Шекель (задача 0)\n";
        cout << "12. Дифференциальная эволюция - Гришагин (задача 1)\n";
        cout << "\n";
        cout << "13. ТЕСТИРОВАНИЕ ВСЕХ ЗАДАЧ ХИЛЛА (ПРЯМОЙ ПЕРЕБОР)\n";
        cout << "14. ТЕСТИРОВАНИЕ ВСЕХ ЗАДАЧ ШЕКЕЛЯ (ПРЯМОЙ ПЕРЕБОР)\n";
        cout << "15. ТЕСТИРОВАНИЕ ВСЕХ ЗАДАЧ ГРИШАГИНА (ПРЯМОЙ ПЕРЕБОР)\n";
        cout << "19. ТЕСТИРОВАНИЕ 2D ЗАДАЧ GKLS (ПРЯМОЙ ПЕРЕБОР)\n";
        cout << "20. ТЕСТИРОВАНИЕ 3D ЗАДАЧ GKLS (ПРЯМОЙ ПЕРЕБОР)\n";
        cout << "21. ТЕСТИРОВАНИЕ 4D ЗАДАЧ GKLS (ПРЯМОЙ ПЕРЕБОР)\n";
        cout << "22. ТЕСТИРОВАНИЕ 5D ЗАДАЧ GKLS (ПРЯМОЙ ПЕРЕБОР)\n";
        cout << "\n";
        cout << "16. ТЕСТИРОВАНИЕ ВСЕХ ЗАДАЧ ХИЛЛА (ДИФФ. ЭВОЛЮЦИЯ)\n";
        cout << "17. ТЕСТИРОВАНИЕ ВСЕХ ЗАДАЧ ШЕКЕЛЯ (ДИФФ. ЭВОЛЮЦИЯ)\n";
        cout << "18. ТЕСТИРОВАНИЕ ВСЕХ ЗАДАЧ ГРИШАГИНА (ДИФФ. ЭВОЛЮЦИЯ)\n";
        cout << "23. ТЕСТИРОВАНИЕ 2D ЗАДАЧ GKLS (ДИФФ. ЭВОЛЮЦИЯ)\n";
        cout << "24. ТЕСТИРОВАНИЕ 3D ЗАДАЧ GKLS (ДИФФ. ЭВОЛЮЦИЯ)\n";
        cout << "25. ТЕСТИРОВАНИЕ 4D ЗАДАЧ GKLS (ДИФФ. ЭВОЛЮЦИЯ)\n";
        cout << "26. ТЕСТИРОВАНИЕ 5D ЗАДАЧ GKLS (ДИФФ. ЭВОЛЮЦИЯ)\n";
        cout << "\n";
        cout << "0. Выход\n \n";
        cout << "Выберите тест: ";

        cin >> choice;

        switch (choice) {
        case 1:
            demoRastriginDirect(2);
            break;
        case 2:
            demoRastriginDirect(3);
            break;
        case 3:
            demoHillDirect(0);
            break;
        case 4:
            demoHillDirect(42);
            break;
        case 5:
            demoShekelDirect(0);
            break;
        case 6:
            demoGrishaginDirect(1);
            break;
        case 7:
            demoRastriginDE(2);
            break;
        case 8:
            demoRastriginDE(3);
            break;
        case 9:
            demoHillDE(0);
            break;
        case 10:
            demoHillDE(42);
            break;
        case 11:
            demoShekelDE(0);
            break;
        case 12:
            demoGrishaginDE(1);
            break;
        case 13:
            testHillDirectAll();
            break;
        case 14:
            testShekelDirectAll();
            break;
        case 15:
            testGrishaginDirectAll();
            break;
        case 16:
            testHillDEAll();
            break;
        case 17:
            testShekelDEAll();
            break;
        case 18:
            testGrishaginDEAll();
            break;

        case 19:
            testGKLSDirect2D();
            break;
        case 20:
            testGKLSDirect3D();
            break;
        case 21:
            testGKLSDirect4D();
            break;
        case 22:
            testGKLSDirect5D();
            break;
        case 23:
            testGKLSDE2D();
            break;
        case 24:
            testGKLSDE3D();
            break;
        case 25:
            testGKLSDE4D();
            break;
        case 26:
            testGKLSDE5D();
            break;
        }

    } while (choice != 0);

    return 0;
}
