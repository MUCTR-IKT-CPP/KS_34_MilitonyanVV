// lab2c.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iomanip>

/**
 * Заполняет массив случайными значениями.
 *
 * @param p_income указатель на массив доходов
 * @param month_count количество месяцев
 */
void fillArray(double* p_income, int month_count)
{
    const double MIN_INCOME = 10.0;
    const double MAX_INCOME = 100.0;

    for (int i = 0; i < month_count; i++)
    {
        double random = (double)rand() / RAND_MAX;
        p_income[i] = MIN_INCOME + random * (MAX_INCOME - MIN_INCOME);
    }
}

/**
 * Выводит массив на экран.
 *
 * @param p_income указатель на массив доходов
 * @param month_count количество месяцев
 */

void printArray(double* p_income, int month_count) {
    std::cout << std::fixed << std::setprecision(1);
    for (int i = 0; i < month_count; i++) {
        std::cout << p_income[i] << " ";
    }
    std::cout << '\n';
}


/**
 * Находит минимальный и максимальный доход.
 *
 * @param p_income указатель на массив доходов
 * @param month_count количество месяцев
 */
void findMinMax(double* p_income, int month_count) {
    double min_income = p_income[0];
    double max_income = p_income[0];
    int min_month = 0;
    int max_month = 0;
    for (int i = 1; i < month_count; i++) {
        if (p_income[i] < min_income) {
            min_income = p_income[i];
            min_month = i;
        }

        if (p_income[i] > max_income) {
            max_income = p_income[i];
            max_month = i;
        }
    }
    std::cout << "Minimum income = " << min_income << " month : " << min_month + 1 << std::endl;
    std::cout << "Maximum income = " << max_income << " month : " << max_month + 1 << std::endl;
}

/**
 * Вычисляет средний доход и стандартное отклонение.
 *
 * @param p_income указатель на массив доходов
 * @param month_count количество месяцев
 */

void calculateStatistics(double* p_income, int month_count) {

    double sum = 0.0;
    for (int i = 0; i < month_count; i++) {
        sum += p_income[i];
    }
    double average_income = sum / month_count;
    double deviation_sum = 0.0;

    for (int i = 0; i < month_count; i++) {
        deviation_sum += (p_income[i] - average_income) * (p_income[i] - average_income);
    }
    double standard_deviation = std::sqrt(deviation_sum / month_count);
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Average income = " << average_income << std::endl;
    std::cout << "Standard deviation = " << standard_deviation << std::endl;

}

/**
 * Сортирует массив по возрастанию.
 *
 * @param p_income указатель на массив доходов
 * @param month_count количество месяцев
 */

void sortArray(double* p_income, int month_count) {
    for (int i = 0; i < month_count - 1; i++) {
        for (int j = 0; j < month_count - 1 - i; j++) {
            if (p_income[j] > p_income[j + 1]) {
                double temp = p_income[j];
                p_income[j] = p_income[j + 1];
                p_income[j + 1] = temp;
            }
        }
    }
}

/**
 * Изменяет указатель, переданный по значению.
 *
 * @param p_income указатель на массив
 */

void changeArrayByPointer(double* p_income)
{
    p_income = nullptr;
}
/**
 * Изменяет указатель, переданный по ссылке.
 *
 * @param p_income ссылка на указатель массива
 */

void changeArrayByReference(double*& p_income)
{
    p_income = nullptr;
}

/**
 * Демонстрирует разницу передачи указателя
 * по значению и по ссылке.
 */
void comparePassingMethods()
{
    std::cout << "\nCompare passing methods\n";

    double* p_array1 = new double[3] {10.0, 20.0, 30.0};

    std::cout << "Before changeArrayByPointer: "
        << p_array1
        << std::endl;

    changeArrayByPointer(p_array1);

    std::cout << "After changeArrayByPointer: "
        << p_array1
        << std::endl;

    delete[] p_array1;

    double* p_array2 = new double[3] {10.0, 20.0, 30.0};

    std::cout << "\nBefore changeArrayByReference: "
        << p_array2
        << std::endl;

    changeArrayByReference(p_array2);

    std::cout << "After changeArrayByReference: "
        << p_array2
        << std::endl;

    if (p_array2 != nullptr)
    {
        delete[] p_array2;
    }
}


int main() {
    srand(time(0));
    int month_count = 0;
    std::cout << "Enter number of months: ";
    std::cin >> month_count;
    if (month_count <= 0) {
        std::cout << "Invalid size!" << std::endl;
        return 1; 
    }
    double* p_income = new double[month_count];
    fillArray(p_income, month_count);
    std::cout << "\nIncome array:\n";
    printArray(p_income, month_count);
    int choice = 0;

    do
    {
        std::cout << "\nChoose operation:\n";
        std::cout << "1 - Min and max income\n";
        std::cout << "2 - Average income and standard deviation\n";
        std::cout << "3 - Sort array\n";
        std::cout << "4 - Compare value and reference passing\n";
        std::cout << "0 - Exit\n";
        std::cout << "Your choice: ";

        std::cin >> choice;

        switch (choice)
        {
        case 1:
            findMinMax(p_income, month_count);
            break;

        case 2:
            calculateStatistics(p_income, month_count);
            break;

        case 3:
            sortArray(p_income, month_count);

            std::cout << "\nSorted array:\n";
            printArray(p_income, month_count);
            break;

        case 4:
            comparePassingMethods();
            break;

        case 0:
            break;

        default:
            std::cout << "Wrong choice!" << std::endl;
        }

    } while (choice != 0);
    delete[] p_income;
    p_income = nullptr;

    return 0;
}




