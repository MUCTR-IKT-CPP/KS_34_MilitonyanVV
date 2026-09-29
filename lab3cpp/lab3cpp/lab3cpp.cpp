#include <iostream>
#include <vector>
#include <string>
#include <ctime>


const int TITLE_COUNT = 5;
const int AUTHOR_COUNT = 5;
const int MIN_YEAR = 1980;
const int MAX_YEAR = 2025;
const int MIN_PAGES = 100;
const int MAX_PAGES = 1000;
const int AVAILABLE_STATES = 2;

struct Book {
    std::string isbn;
    std::string title;
    std::string author;
    int year;
    bool is_available;
    int pages;
};

/**
 * Генерирует случайный ISBN.
 *
 * @return случайный ISBN.
 */
std::string generateIsbn() {
    std::string isbn = "";

    for (int i = 0; i < 3; i++) {
        isbn += char('0' + rand() % 10);
    }
    isbn += "-";
    isbn += char('0' + rand() % 10);
    isbn += "-";
    for (int i = 0; i < 3; i++) {
        isbn += char('0' + rand() % 10);
    }
    isbn += "-";
    for (int i = 0; i < 5; i++) {
        isbn += char('0' + rand() % 10);
    }
    isbn += "-";
    isbn += char('0' + rand() % 10);

    return isbn;
}

/**
 * Заполняет каталог случайными книгами.
 *
 * @param p_books указатель на массив книг
 * @param book_count количество книг
 */
void fillBooks(Book* p_books, int book_count) {
    const std::string TITLES[] = {
        "C++ Basics",
        "Algorithms",
        "Data Structures",
        "Physics",
        "Mathematics"
    };

    const std::string AUTHORS[] = {
        "Ivanov",
        "Petrov",
        "Sidorov",
        "Smirnov",
        "Volkov"
    };

    for (int i = 0; i < book_count; i++) {
        p_books[i].isbn = generateIsbn();
        p_books[i].title = TITLES[rand() % TITLE_COUNT];
        p_books[i].author = AUTHORS[rand() % AUTHOR_COUNT];
        p_books[i].year = MIN_YEAR + rand() % (MAX_YEAR - MIN_YEAR + 1);
        p_books[i].is_available = (rand() % AVAILABLE_STATES) == 1;
        p_books[i].pages = MIN_PAGES + rand() % (MAX_PAGES - MIN_PAGES + 1);
    }
}

/**
 * Вывод каталога.
 *
 * @param p_books указатель на массив книг
 * @param book_count количество книг
 */
void printBooks(const Book* p_books, int book_count) {
    for (int i = 0; i < book_count; i++) {
        std::cout << "\nISBN: " << p_books[i].isbn;
        std::cout << "\nTitle: " << p_books[i].title;
        std::cout << "\nAuthor: " << p_books[i].author;
        std::cout << "\nYear: " << p_books[i].year;
        std::cout << "\nPages: " << p_books[i].pages;
        std::cout << "\nAvailable: ";

        if (p_books[i].is_available) {
            std::cout << "Yes";
        }
        else {
            std::cout << "No";
        }

        std::cout << "\n";
    }
}

/**
 * Поиск по названию или автору.
 *
 * @param p_books указатель на массив книг
 * @param book_count количество книг
 * @param text строка поиска
 */
void searchBooks(const Book* p_books, int book_count, const std::string& text) {
    bool found = false;

    for (int i = 0; i < book_count; i++) {
        if (
            p_books[i].title.find(text) != std::string::npos ||
            p_books[i].author.find(text) != std::string::npos
            ) {
            std::cout << p_books[i].title
                << " - "
                << p_books[i].author
                << std::endl;
            found = true;
        }
    }

    if (!found) {
        std::cout << "No books found\n";
    }
}

/**
 * Фильтр по году.
 *
 * @param p_books указатель на массив книг
 * @param book_count количество книг
 * @param year_from начало периода
 * @param year_to конец периода
 */
void filterByYear(const Book* p_books, int book_count, int year_from, int year_to) {
    bool found = false;

    for (int i = 0; i < book_count; i++) {
        if (p_books[i].year >= year_from && p_books[i].year <= year_to) {
            std::cout << p_books[i].title
                << " "
                << p_books[i].year
                << std::endl;
            found = true;
        }
    }

    if (!found) {
        std::cout << "No books in this year range\n";
    }
}

/**
 * Статистика каталога.
 *
 * @param p_books указатель на массив книг
 * @param book_count количество книг
 */
void printStatistics(const Book* p_books, int book_count) {
    int available_count = 0;
    int pages_sum = 0;

    for (int i = 0; i < book_count; i++) {
        pages_sum += p_books[i].pages;

        if (p_books[i].is_available) {
            available_count++;
        }
    }

    double average_pages = static_cast<double>(pages_sum) / book_count;
    int unavailable_count = book_count - available_count;
    std::cout << "\nTotal books: " << book_count;
    std::cout << "\nAverage pages: " << average_pages;
    std::cout << "\nAvailable: " << available_count;
    std::cout << "\nUnavailable: " << unavailable_count;
    std::cout << std::endl;
}

/**
 * Сортировка по году и автору.
 *
 * @param p_books указатель на массив книг
 * @param book_count количество книг
 */
void sortBooks(Book* p_books, int book_count) {
    for (int i = 0; i < book_count - 1; i++) {
        for (int j = 0; j < book_count - 1; j++) {
            if (
                p_books[j].year < p_books[j + 1].year ||
                (
                    p_books[j].year == p_books[j + 1].year &&
                    p_books[j].author > p_books[j + 1].author
                    )
                ) {
                Book temp = p_books[j];
                p_books[j] = p_books[j + 1];
                p_books[j + 1] = temp;
            }
        }
    }
}

/**
 * Выдача книги.
 *
 * @param p_books указатель на массив книг
 * @param book_count количество книг
 * @param isbn ISBN книги
 */
void giveBook(Book* p_books, int book_count, const std::string& isbn) {
    for (int i = 0; i < book_count; i++) {
        if (p_books[i].isbn == isbn) {
            if (p_books[i].is_available) {
                p_books[i].is_available = false;
                std::cout << "Book issued\n";
            }
            else {
                std::cout << "Book already issued\n";
            }

            return;
        }
    }

    std::cout << "Book not found\n";
}

/**
 * Возврат книги.
 *
 * @param p_books указатель на массив книг
 * @param book_count количество книг
 * @param isbn ISBN книги
 */
void returnBook(Book* p_books, int book_count, const std::string& isbn) {
    for (int i = 0; i < book_count; i++) {
        if (p_books[i].isbn == isbn) {
            if (!p_books[i].is_available) {
                p_books[i].is_available = true;
                std::cout << "Book returned\n";
            }
            else {
                std::cout << "Book already in library\n";
            }

            return;
        }
    }

    std::cout << "Book not found\n";
}

/**
 * Выводит меню действий на экран.
 */
void printMenu() {
    std::cout << "\n1. Show all books\n";
    std::cout << "2. Search by title or author\n";
    std::cout << "3. Filter by year\n";
    std::cout << "4. Show statistics\n";
    std::cout << "5. Sort books\n";
    std::cout << "6. Issue book\n";
    std::cout << "7. Return book\n";
    std::cout << "0. Exit\n";
    std::cout << "Your choice: ";
}

int main() {
    srand(time(0));

    int book_count = 0;

    std::cout << "Enter number of books: ";
    std::cin >> book_count;

    if (book_count <= 0) {
        std::cout << "Invalid number of books. Exiting.\n";
        return 1;
    }

    Book* p_books = new Book[book_count];

    fillBooks(p_books, book_count);

    int choice = -1;

    do {
        printMenu();
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid input. Try again.\n";
            continue;
        }

        switch (choice) {
        case 1: {
            std::cout << "\nCATALOG\n";
            printBooks(p_books, book_count);
            break;
        }
        case 2: {
            std::string text;
            std::cout << "\nSearch text: ";
            std::cin >> text;
            searchBooks(p_books, book_count, text);
            break;
        }
        case 3: {
            int year_from = 0;
            int year_to = 0;
            std::cout << "\nEnter year range: ";
            std::cin >> year_from >> year_to;
            filterByYear(p_books, book_count, year_from, year_to);
            break;
        }
        case 4: {
            printStatistics(p_books, book_count);
            break;
        }
        case 5: {
            sortBooks(p_books, book_count);
            std::cout << "\nSORTED CATALOG\n";
            printBooks(p_books, book_count);
            break;
        }
        case 6: {
            std::string isbn;
            std::cout << "\nEnter ISBN to issue: ";
            std::cin >> isbn;
            giveBook(p_books, book_count, isbn);
            break;
        }
        case 7: {
            std::string isbn;
            std::cout << "\nEnter ISBN to return: ";
            std::cin >> isbn;
            returnBook(p_books, book_count, isbn);
            break;
        }
        case 0: {
            std::cout << "\nExiting...\n";
            break;
        }
        default: {
            std::cout << "\nInvalid choice. Try again.\n";
            break;
        }
        }
    } while (choice != 0);

    delete[] p_books;

    return 0;
}