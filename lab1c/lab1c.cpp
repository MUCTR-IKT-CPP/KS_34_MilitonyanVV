// lab1c.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <string>
#include <ctime>

using namespace std;

/*
 * Генерирует случайную строку из букв a-z.
 *
 * @param n длина строки.
 * @return возвращает случайную строку длины n.
 */
string generateString(int n)
{
    string s = "";

    for (int i = 0; i < n; i++) {
        char letter = 'a' + rand() % 26;
        s += letter;
    }

    return s;
}

/*
 * Подсчитывает количество каждой буквы в строке.
 *
 * @param s анализируемая строка.
 * @param count массив для хранения количества букв.
 * @param n длина строки.
 */
void countLetters(string s, int count[], int n)
{
    for (int i = 0; i < 26; i++) {
        count[i] = 0;
    }

    for (int i = 0; i < n; i++) {
        int index = s[i] - 'a';
        count[index]++;
    }
}

/*
 * Находит букву, которая встречается чаще всего.
 *
 * @param count массив количеств букв.
 * @return возвращает наиболее часто встречающуюся букву (по списку алфавита).
 */
char findMostFrequent(int count[])
{
    int max_index = 0;

    for (int i = 1; i < 26; i++) {
        if (count[i] > count[max_index]) {
            max_index = i;
        }
    }

    return 'a' + max_index;
}

/*
 * Выводит гистограмму частот букв.
 *
 * @param count массив количеств букв.
 */
void printHistogram(int count[])
{
    for (int i = 0; i < 26; i++) {
        if (count[i] > 0) {
            cout << char('a' + i) << ": ";

            for (int j = 0; j < count[i]; j++) {
                cout << '*';
            }

            cout << '\n';
        }
    }
}

int main()
{

    srand(time(0));

    const int N = 100;

    string generated_string = generateString(N);

    cout << "String: " << generated_string << '\n';

    int letter_count[26];

    countLetters(generated_string, letter_count, N);

    cout << '\n';
    cout << "Histogram:\n";

    printHistogram(letter_count);

    cout << '\n';

    char most_frequent_letter = findMostFrequent(letter_count);

    cout << "Most frequent letter: " << most_frequent_letter << '\n';

    return 0;
}