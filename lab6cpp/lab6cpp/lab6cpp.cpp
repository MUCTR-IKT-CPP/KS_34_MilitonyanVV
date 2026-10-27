#include <iostream>
#include <string>
#include <vector>

class Animal {
private:
    int _id;

public:
    /**
     * @brief Создаёт базовую часть животного.
     * @param id Идентификатор животного.
     */
    Animal(int id)
        : _id(id) {
    }

    int getId() const {
        return _id;
    }

    virtual void eat() const = 0;
    virtual void sleep() const = 0;
    virtual void makeSound() const = 0;
    virtual ~Animal() = default;
};

class IMovable {
public:
    virtual void move() const = 0;
    virtual ~IMovable() = default;
};

class Mammal : public Animal {
private:
    std::string _furColor;

public:
    /**
     * @param id Идентификатор животного.
     * @param furColor Цвет шерсти.
     */
    Mammal(int id, const std::string& furColor)
        : Animal(id), _furColor(furColor) {
    }
    void eat() const override {
        std::cout << "Mammal eats food.\n";
    }
    void sleep() const override {
        std::cout << "Mammal sleeps.\n";
    }
    void makeSound() const override {
        std::cout << "Mammal (" << _furColor << " fur): growl!\n";
    }
};

class Bird : public Animal, public IMovable {
private:
    double _wingspan;

public:
    /**
     * @param id Идентификатор животного.
     * @param wingspan Положительный размах крыльев в метрах.
     */
    Bird(int id, double wingspan)
        : Animal(id), _wingspan(wingspan) {
    }
    void eat() const override {
        std::cout << "Bird eats seeds.\n";
    }
    void sleep() const override {
        std::cout << "Bird sleeps in a nest.\n";
    }
    void makeSound() const override {
        std::cout << "Bird (wingspan " << _wingspan
            << " m): chirp!\n";
    }
    void move() const override {
        std::cout << "Bird flies.\n";
    }
};

class Reptile : public Animal, public IMovable {
private:
    std::string _scaleType;

public:
    /**
     * @param id Идентификатор животного.
     * @param scaleType Тип чешуи.
     */
    Reptile(int id, const std::string& scaleType)
        : Animal(id), _scaleType(scaleType) {
    }
    void eat() const override {
        std::cout << "Reptile eats insects.\n";
    }
    void sleep() const override {
        std::cout << "Reptile sleeps in a shelter.\n";
    }
    void makeSound() const override {
        std::cout << "Reptile (" << _scaleType
            << " scales): hiss!\n";
    }
    void move() const override {
        std::cout << "Reptile crawls.\n";
    }
};

class Zoo {
private:
    std::vector<Animal*> _animals;

public:
    /**
     * @brief Добавляет животное с уникальным ID.
     * @param anim Указатель на существующее животное.
     * @return true при добавлении, false при nullptr или повторе ID.
     * @note Животное должно существовать всё время хранения в Zoo.
     */
    bool addAnimal(Animal* anim) {
        if (anim == nullptr) {
            return false;
        }

        for (Animal* animal : _animals) {
            if (animal->getId() == anim->getId()) {
                return false;
            }
        }

        _animals.push_back(anim);

        return true;
    }

    /**
     * @brief Убирает животное из зоопарка, сохраняя сам объект.
     * @param id Идентификатор животного.
     * @return true при удалении, false, если ID не найден.
     */
    bool removeAnimal(int id) {
        for (std::size_t i = 0; i < _animals.size(); i++) {
            if (_animals[i]->getId() == id) {
                _animals.erase(_animals.begin() + i);

                return true;
            }
        }

        return false;
    }

    void printAll() const {
        if (_animals.empty()) {
            std::cout << "Zoo is empty.\n";
            return;
        }

        for (Animal* animal : _animals) {
            std::cout << "ID: " << animal->getId() << " | ";
            animal->makeSound();
        }
    }
    /**
     * @brief Вызывает питание, звук, доступное перемещение и сон.
     * @return Ничего не возвращает.
     */
    void simulateDay() const {
        if (_animals.empty()) {
            std::cout << "Zoo is empty.\n";
            return;
        }

        for (Animal* animal : _animals) {
            std::cout << "\nAnimal ID: " << animal->getId() << '\n';

            animal->eat();
            animal->makeSound();

            IMovable* movable = dynamic_cast<IMovable*>(animal);

            if (movable != nullptr) {
                movable->move();
            }
            animal->sleep();
        }
    }
};

/**
 * @brief Читает целое число, повторяя ввод при ошибке.
 * @param prompt Приглашение к вводу.
 * @param value Переменная для записи числа.
 * @return true при успешном вводе, false при конце ввода.
 */
template <typename T>
bool readNumber(const char* prompt, T& value) {
    while (true) {
        std::cout << prompt;

        if (std::cin >> value) {
            return true;
        }

        if (std::cin.eof()) {
            return false;
        }

        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cout << "Invalid number. Try again.\n";
    }
}
/**
 * @brief Создаёт животное по данным пользователя.
 * @param id Идентификатор нового животного.
 * @return Указатель на животное или nullptr при неправильных данных.
 */
Animal* createAnimal(int id) {
    int type;

    if (!readNumber("Type (1 Mammal, 2 Bird, 3 Reptile): ", type)) {
        return nullptr;
    }

    if (type == 2) {
        double wingspan;

        if (!readNumber("Wingspan in meters: ", wingspan)) {
            return nullptr;
        }
        if (wingspan <= 0) {
            std::cout << "Wingspan must be positive.\n";
            return nullptr;
        }
        
        return new Bird(id, wingspan);
    }

    if (type == 1 || type == 3) {
        std::string feature;

        if (type == 1) {
            std::cout << "Fur color: ";
        }
        else {
            std::cout << "Scale type: ";
        }

        if (!std::getline(std::cin >> std::ws, feature)) {
            return nullptr;
        }

        if (type == 1) {
            return new Mammal(id, feature);
        }

        return new Reptile(id, feature);
    }

    std::cout << "Unknown animal type.\n";
    return nullptr;
}
int main() {
    std::vector<Animal*> createdAnimals;
    Zoo zoo;

    Animal* initialAnimals[] = {
        new Mammal(1, "Brown"),
        new Bird(2, 1.5),
        new Reptile(3, "Smooth")
    };

    for (Animal* animal : initialAnimals) {
        createdAnimals.push_back(animal);
        zoo.addAnimal(animal);
    }

    int nextId = 4;
    bool running = true;

    while (running) {
        std::cout << "\nZOO MENU\n"
            << "1. Add animal\n"
            << "2. Remove animal\n"
            << "3. Show animals and sounds\n"
            << "4. Simulate day\n"
            << "0. Exit\n";

        int choice;

        if (!readNumber("Choice: ", choice)) {
            break;
        }

        switch (choice) {
        case 1: {
            Animal* animal = createAnimal(nextId);

            if (animal == nullptr) {
                break;
            }

            if (zoo.addAnimal(animal)) {
                createdAnimals.push_back(animal);
                std::cout << "Added animal. ID: " << nextId << '\n';
                nextId++;
            }
            else {
                delete animal;
                std::cout << "Cannot add animal.\n";
            }

            break;
        }

        case 2: {
            int id;

            if (!readNumber("Animal ID: ", id)) {
                running = false;
                break;
            }

            if (zoo.removeAnimal(id)) {
                std::cout << "Animal removed from zoo.\n";
            }
            else {
                std::cout << "Animal not found.\n";
            }

            break;
        }

        case 3:
            zoo.printAll();
            break;

        case 4:
            zoo.simulateDay();
            break;

        case 0:
            running = false;
            break;

        default:
            std::cout << "Unknown menu item.\n";
        }
    }

    for (Animal* animal : createdAnimals) {
        delete animal;
    }

    return 0;
}