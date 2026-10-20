
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <cstdlib>
#include <ctime>

const int DAYS = 5;
const int PAIRS = 6;
const int MAX_DAILY_PAIRS = 4;
const int MAX_COURSES = 12;
const int MIN_COURSES = 8;
const int WEEKS = 16;
const int GROUPS = 3;
const int TEACHERS = 6;
const int MAX_LESSONS = MAX_COURSES * 2;
const int SEARCH_ATTEMPTS = 1000;

const char* const DAY_NAMES[DAYS] = { "Mon", "Tue", "Wed", "Thu", "Fri" };
const char* const TEACHER_NAMES[TEACHERS] = {
    "Ivanov", "Petrova", "Smirnov", "Orlova", "Volkov", "Sokolova"
};

/**
 * @brief Возвращает случайное целое число в заданных границах.
 * @param minimum Нижняя граница включительно.
 * @param maximum Верхняя граница включительно.
 * @return Случайное целое число от minimum до maximum включительно.
 */
int randomInt(int minimum, int maximum) {
    return minimum + std::rand() % (maximum - minimum + 1);
}

enum class CourseType {
    PROGRAMMING,
    MATH,
    PHYSICS
};

enum class PreferredTime {
    MORNING,
    AFTERNOON
};

class Course {
private:
    std::string _name;
    int _durationHours;
    int _difficulty;
    CourseType _type;
    PreferredTime _preferredTime;
    int _lessonPairs;
    int _frequency;
    int _teacher;
protected:
    /**
     * @brief Переводит результат экзамена в оценку от 2 до 5.
     * @param score Результат экзамена в баллах.
     * @return Оценка от 2 до 5.
     */
    int gradeFromScore(double score) const {
        if (score >= 85) return 5;
        if (score >= 65) return 4;
        if (score >= 50) return 3;
        return 2;
    }
public:
    /**
     * @brief Создаёт курс, проверяет параметры и рассчитывает часы семестра.
     * @param name Название курса или группы.
     * @param difficulty Сложность курса от 1 до 5.
     * @param type Вид курса.
     * @param preferredTime Предпочтительное время занятий.
     * @param lessonPairs Количество пар подряд в одном занятии, от 1 до 3.
     * @param frequency Количество занятий в неделю, от 1 до 2.
     * @param teacher Индекс преподавателя, начиная с 0.
     */
    Course(const std::string& name, int difficulty, CourseType type,
        PreferredTime preferredTime, int lessonPairs, int frequency,
        int teacher)
        : _name(name), _durationHours(0),
        _difficulty(difficulty), _type(type),
        _preferredTime(preferredTime),
        _lessonPairs(lessonPairs), _frequency(frequency),
        _teacher(teacher) {

        if (name.empty() || difficulty < 1 || difficulty > 5 ||
            lessonPairs < 1 || lessonPairs > 3 ||
            frequency < 1 || frequency > 2 ||
            teacher < 0 || teacher >= TEACHERS) {
            throw std::invalid_argument("Invalid course parameters.");
        }

        if (preferredTime != PreferredTime::MORNING &&
            preferredTime != PreferredTime::AFTERNOON) {
            throw std::invalid_argument("Unknown preferred time.");
        }

        _durationHours = WEEKS * frequency * lessonPairs * 2;
    }

    /**
     * @brief Возвращает название курса без копирования строки.
     * @return Константная ссылка на название курса.
     */
    const std::string& getName() const { return _name; }
    /**
     * @brief Возвращает количество академических часов за семестр.
     * @return Запланированное количество академических часов за семестр.
     */
    int getDurationHours() const { return _durationHours; }
    /**
     * @brief Возвращает сложность курса от 1 до 5.
     * @return Сложность курса от 1 до 5.
     */
    int getDifficulty() const { return _difficulty; }
    /**
     * @brief Возвращает вид курса.
     * @return Вид курса.
     */
    CourseType getType() const { return _type; }
    /**
     * @brief Возвращает предпочтительную половину учебного дня.
     * @return Предпочтительное время занятий.
     */
    PreferredTime getPreferredTime() const { return _preferredTime; }

    int getLessonPairs() const { return _lessonPairs; }
    int getFrequency() const { return _frequency; }
    int getTeacher() const { return _teacher; }
    virtual void study() const = 0;
    /**
     * @brief Задаёт общий интерфейс экзамена с возвратом оценки.
     * @param knowledge Уровень знаний перед экзаменом.
     * @return Экзаменационная оценка от 2 до 5.
     */
    virtual int checkKnowledge(double knowledge) const = 0;

    /**
     * @brief Обеспечивает корректное уничтожение наследников через Course*.
     */
    virtual ~Course() = default;

    /**
     * @brief Выводит характеристики курса в переданный поток.
     * @param output Поток для вывода характеристик курса.
     * @param course Курс, используемый функцией.
     * @return Ссылка на тот же поток для продолжения цепочки вывода.
     */
    friend std::ostream& operator<<(std::ostream& output, const Course& course) {
        const char* types[3] = { "Programming", "Math", "Physics" };
        output << std::left
            << std::setw(16) << "Type:"
            << types[static_cast<int>(course.getType())] << '\n'
            << std::setw(16) << "Hours:" << course.getDurationHours() << '\n'
            << std::setw(16) << "Difficulty:" << course.getDifficulty() << '\n'
            << std::setw(16) << "Pairs/lesson:" << course.getLessonPairs() << '\n'
            << std::setw(16) << "Lessons/week:" << course.getFrequency() << '\n'
            << std::setw(16) << "Preference:"
            << (course.getPreferredTime() == PreferredTime::MORNING
                ? "morning" : "afternoon") << '\n'
            << std::setw(16) << "Teacher:"
            << TEACHER_NAMES[course.getTeacher()];

        return output;
    }
};

class ProgrammingCourse : public Course {
private:
    std::string _language;

public:
    /**
     * @brief Создаёт курс программирования с заданным языком.
     * @param name Название курса или группы.
     * @param difficulty Сложность курса от 1 до 5.
     * @param time Предпочтительное время занятий.
     * @param pairs Количество пар подряд в одном занятии, от 1 до 3.
     * @param frequency Количество занятий в неделю, от 1 до 2.
     * @param teacher Индекс преподавателя, начиная с 0.
     * @param language Язык программирования.
     */
    ProgrammingCourse(const std::string& name, int difficulty,
        PreferredTime time, int pairs, int frequency,
        int teacher, const std::string& language)
        : Course(name, difficulty, CourseType::PROGRAMMING,
            time, pairs, frequency, teacher),
        _language(language) {

        if (language.empty()) {
            throw std::invalid_argument("Language is required.");
        }
    }
    /**
     * @brief Выводит методику обучения через написание и отладку программ.
     */
    void study() const override {
        std::cout << "Description: writing and debugging programs in "
            << _language << ".\n";
    }
    /**
     * @brief Рассчитывает оценку практического экзамена по программированию.
     * @param knowledge Уровень знаний перед экзаменом.
     * @return Экзаменационная оценка от 2 до 5.
     */
    int checkKnowledge(double knowledge) const override {
        double score = knowledge + randomInt(-8, 8);
        return gradeFromScore(score);
    }
};
class MathCourse : public Course {
private:
    std::string _section;

public:
    /**
     * @brief Создаёт математический курс с заданным разделом.
     * @param name Название курса или группы.
     * @param difficulty Сложность курса от 1 до 5.
     * @param time Предпочтительное время занятий.
     * @param pairs Количество пар подряд в одном занятии, от 1 до 3.
     * @param frequency Количество занятий в неделю, от 1 до 2.
     * @param teacher Индекс преподавателя, начиная с 0.
     * @param section Раздел математики.
     */
    MathCourse(const std::string& name, int difficulty,
        PreferredTime time, int pairs, int frequency,
        int teacher, const std::string& section)
        : Course(name, difficulty, CourseType::MATH,
            time, pairs, frequency, teacher),
        _section(section) {

        if (section.empty()) {
            throw std::invalid_argument("Math section is required.");
        }
    }
    /**
     * @brief Выводит методику обучения через теорию и решение задач.
     */
    void study() const override {
        std::cout << "Description: theory and problem solving; section: "
            << _section << ".\n";
    }
    /**
     * @brief Рассчитывает оценку письменного экзамена по математике.
     * @param knowledge Уровень знаний перед экзаменом.
     * @return Экзаменационная оценка от 2 до 5.
     */
    int checkKnowledge(double knowledge) const override {
        return gradeFromScore(knowledge + randomInt(-10, 6));
    }
};

class PhysicsCourse : public Course {
private:
    int _laboratoryCount;

public:
    /**
     * @brief Создаёт курс физики с заданным числом лабораторных.
     * @param name Название курса или группы.
     * @param difficulty Сложность курса от 1 до 5.
     * @param time Предпочтительное время занятий.
     * @param pairs Количество пар подряд в одном занятии, от 1 до 3.
     * @param frequency Количество занятий в неделю, от 1 до 2.
     * @param teacher Индекс преподавателя, начиная с 0.
     * @param laboratoryCount Количество лабораторных работ за семестр.
     */
    PhysicsCourse(const std::string& name, int difficulty,
        PreferredTime time, int pairs, int frequency,
        int teacher, int laboratoryCount)
        : Course(name, difficulty, CourseType::PHYSICS,
            time, pairs, frequency, teacher),
        _laboratoryCount(laboratoryCount) {

        if (laboratoryCount < 1 ||
            laboratoryCount > WEEKS * frequency) {
            throw std::invalid_argument(
                "Invalid number of laboratory assignments.");
        }
    }
    /**
     * @brief Выводит методику обучения через теорию и эксперименты.
     */
    void study() const override {
        std::cout << "Description: theory and experiments; laboratory assignments: "
            << _laboratoryCount << ".\n";
    }

    /**
     * @brief Объединяет теорию и практику и возвращает оценку по физике.
     * @param knowledge Уровень знаний перед экзаменом.
     * @return Экзаменационная оценка от 2 до 5.
     */
    int checkKnowledge(double knowledge) const override {
        const double theory = knowledge + randomInt(-8, 8);
        const double practice = knowledge + randomInt(-6, 10);

        return gradeFromScore(theory * 0.5 + practice * 0.5);
    }
};

class StudyPlan {
private:
    Course* _courses[MAX_COURSES]{};
    int _count = 0;
    int _taughtHours[MAX_COURSES]{};
public:
    /**
     * @brief Создаёт пустой учебный план.
     */
    StudyPlan() = default;
    /**
     * @brief Запрещает копирование плана, владеющего указателями на курсы.
     * @note Параметр копирования намеренно не назван: операция запрещена.
     */
    StudyPlan(const StudyPlan&) = delete;
    /**
     * @brief Запрещает копирующее присваивание плана, владеющего курсами.
     * @note Параметр копирования намеренно не назван: операция запрещена.
     */
    StudyPlan& operator=(const StudyPlan&) = delete;
    /**
     * @brief Принимает владение курсом; при переполнении удаляет переданный объект.
     * @param course Указатель на курс; план принимает владение объектом.
     */
    void addCourse(Course* course) {
        if (course == nullptr) {
            throw std::invalid_argument("Null course pointer.");
        }

        if (_count >= MAX_COURSES) {
            delete course;
            throw std::runtime_error("The study plan is full.");
        }

        _courses[_count++] = course;
    }
    /**
     * @brief Обнуляет количество проведённых часов по всем курсам.
     */
    void resetProgress() {
        for (int i = 0; i < MAX_COURSES; i++) {
            _taughtHours[i] = 0;
        }
    }
    /**
     * @brief Удаляет курсы и сбрасывает количество курсов и проведённые часы.
     */
    void clear() {
        for (int i = 0; i < _count; i++) {
            delete _courses[i];
            _courses[i] = nullptr;
        }

        _count = 0;
        resetProgress();
    }
    /**
     * @brief Освобождает все курсы, принадлежащие учебному плану.
     */
    ~StudyPlan() {
        clear();
    }
    /**
     * @brief Возвращает количество курсов в учебном плане.
     * @return Количество курсов в плане.
     */
    int getCount() const {
        return _count;
    }
    /**
     * @brief Проверяет индекс и возвращает константную ссылку на курс.
     * @param index Индекс курса, начиная с 0.
     * @return Константная ссылка на найденный курс.
     */
    const Course& getCourse(int index) const {
        if (index < 0 || index >= _count) {
            throw std::out_of_range("Invalid course index.");
        }
        return *_courses[index];
    }
    /**
     * @brief Рассчитывает суммарную недельную нагрузку в парах.
     * @return Суммарное количество учебных пар в неделю.
     */
    int weeklyPairs() const {
        int total = 0;

        for (int i = 0; i < _count; i++) {
            total += _courses[i]->getLessonPairs()
                * _courses[i]->getFrequency();
        }

        return total;
    }
    /**
     * @brief Учитывает часы одного занятия, запрещая превышение плана.
     * @param index Индекс курса, начиная с 0.
     */
    void recordLesson(int index) {
        const Course& course = getCourse(index);
        const int hours = course.getLessonPairs() * 2;

        if (_taughtHours[index] + hours > course.getDurationHours()) {
            throw std::logic_error("Course duration exceeded.");
        }

        _taughtHours[index] += hours;
    }
    /**
     * @brief Создаёт случайные курсы с ограничением недельной нагрузки.
     */
    void generate() {
        clear();

        const char* subjects[3][3] = {
            {"C++", "Python", "Java"},
            {"Algebra", "Geometry", "Analysis"},
            {"Mechanics", "Optics", "Electricity"}
        };

        const int count = randomInt(MIN_COURSES, MAX_COURSES);
        int budget = DAYS * MAX_DAILY_PAIRS; //сколько учебных пар ещё можно распределить

        for (int i = 0; i < count; i++) {
            const int available = budget - (count - i - 1);

            int pairs;
            int frequency;

            do {
                pairs = randomInt(1, 3);
                frequency = randomInt(1, 2);
            } while (pairs * frequency > available);

            budget -= pairs * frequency;

            const int difficulty = randomInt(1, 5);

            const PreferredTime time = randomInt(0, 1) == 0
                ? PreferredTime::MORNING
                : PreferredTime::AFTERNOON;

            const int teacher = randomInt(0, TEACHERS - 1);

            const int kind = i < 3 ? i : randomInt(0, 2);

            const std::string subject =
                subjects[kind][randomInt(0, 2)];

            const std::string name =
                subject + "-" + std::to_string(i + 1);

            if (kind == 0) {
                addCourse(new ProgrammingCourse(
                    name, difficulty, time,
                    pairs, frequency, teacher, subject));
            }
            else if (kind == 1) {
                addCourse(new MathCourse(
                    name, difficulty, time,
                    pairs, frequency, teacher, subject));
            }
            else {
                addCourse(new PhysicsCourse(
                    name, difficulty, time,
                    pairs, frequency, teacher, randomInt(2, 8)));
            }
        }
    }
    /**
     * @brief Выводит курсы, методики обучения и общую нагрузку.
     */
    void print() const {
        std::cout << "\nSTUDY PLAN\n";
        for (int i = 0; i < _count; i++) {
            std::cout << "\n" << i + 1 << ". " << _courses[i]->getName() << '\n';
            std::cout << "-----------------------\n";
            std::cout << *_courses[i] << '\n';
            _courses[i]->study();
        }

        std::cout << "\n-----------------------\n";
        std::cout << "Weekly workload: "
            << weeklyPairs() << " pairs = " << weeklyPairs() * 2 << " academic hours.\n"
            << "Semester workload: " << weeklyPairs() * 2 * WEEKS << " hours.\n";
    }
    /**
     * @brief Выводит проведённые часы и процент выполнения каждого курса.
     */
    void printProgress() const {
        std::cout << "\nSTUDY PLAN COMPLETION\n";
        for (int i = 0; i < _count; i++) {
            const int planned = _courses[i]->getDurationHours();
            std::cout << _courses[i]->getName() << ": " << _taughtHours[i]
                << '/' << planned << " hours, "
                << 100.0 * _taughtHours[i] / planned << "%\n";
        }
    }
};

class Schedule {
private:
    int _slots[DAYS][PAIRS];
    /**
     * @brief Помечает все ячейки недельного расписания как свободные.
     */
    void clearSlots() {
        for (int d = 0; d < DAYS; d++) {
            for (int p = 0; p < PAIRS; p++) {
                _slots[d][p] = -1;
            }
        }
    }
    /**
     * @brief Проверяет ограничения размещения занятия, отменяя пробную запись.
     * @param plan Учебный план с курсами.
     * @param id Индекс курса, начиная с 0.
     * @param day Индекс дня недели от 0 до DAYS - 1.
     * @param start Индекс первой пары занятия, начиная с 0.
     * @return true, если занятие можно разместить с соблюдением ограничений; иначе false.
     */
    bool canPlace(const StudyPlan& plan, int id, int day, int start) {
        const Course& course = plan.getCourse(id);
        const int length = course.getLessonPairs();
        if (start + length > PAIRS || dayLoad(day) + length > MAX_DAILY_PAIRS) return false;
        if (course.getDifficulty() >= 4 && start + length > 3) return false;
        for (int p = 0; p < PAIRS; p++) {
            if (_slots[day][p] == id) return false;
        }
        for (int p = start; p < start + length; p++) {
            if (_slots[day][p] != -1) return false;
        }
        // Проверяем перерывы и при добавлении простого курса
        place(day, start, length, id);
        const bool valid = hasHardBreaks(plan, day);
        place(day, start, length, -1);
        return valid;
    }
    /**
     * @brief Проверяет свободные перерывы между разными сложными занятиями.
     * @param plan Учебный план с курсами.
     * @param day Индекс дня недели от 0 до DAYS - 1.
     * @return true, если между разными сложными занятиями есть свободная пара; иначе false.
     */
    bool hasHardBreaks(const StudyPlan& plan, int day) const {
        bool hadHard = false; //уже встретили сложное занятие?
        bool hadBreak = false;//после него была свободная пара
        for (int p = 0; p < PAIRS; p++) {
            const int id = _slots[day][p];
            if (id == -1) { hadBreak = true; continue; }
            if (plan.getCourse(id).getDifficulty() < 4) continue;
            if (p > 0 && _slots[day][p - 1] == id) continue;
            if (hadHard && !hadBreak) return false;
            hadHard = true;
            hadBreak = false;
        }
        return true;
    }
    /**
     * @brief Записывает индекс курса или -1 в непрерывный блок пар.
     * @param day Индекс дня недели от 0 до DAYS - 1.
     * @param start Индекс первой пары занятия, начиная с 0.
     * @param length Количество пар в записываемом блоке.
     * @param id Индекс курса или -1 для освобождения блока.
     */
    void place(int day, int start, int length, int id) {
        for (int p = start; p < start + length; p++) {
            _slots[day][p] = id;
        }
    }

    /**
     * @brief Проверяет попадание всего занятия в предпочтительное время.
     * @param course Курс, используемый функцией.
     * @param start Индекс первой пары занятия, начиная с 0.
     * @return true, если всё занятие попадает в предпочтительное время; иначе false.
     */
    bool matchesPreference(const Course& course, int start) const {
        if (course.getPreferredTime() == PreferredTime::MORNING) {
            return start + course.getLessonPairs() <= 3;
        }
        return start >= 3;
    }
    /**
     * @brief Считает штраф за окна, неравномерную нагрузку и нарушение предпочтений.
     * @param plan Учебный план с курсами.
     * @return Общий штраф расписания: меньшее значение считается лучшим.
     */
    int score(const StudyPlan& plan) const {
        int result = windowCount() * 100;

        for (int d = 0; d < DAYS; d++) {
            result += dayLoad(d) * dayLoad(d) * 10;

            for (int p = 0; p < PAIRS; p++) {
                const int id = _slots[d][p];

                if (id != -1 &&
                    (p == 0 || _slots[d][p - 1] != id) &&
                    !matchesPreference(plan.getCourse(id), p)) {
                    result += 8;
                }
            }
        }

        return result;
    }
public:
    /**
     * @brief Создаёт пустое недельное расписание.
     */
    Schedule() {
        clearSlots();
    }
    /**
     * @brief Возвращает количество занятых учебных пар в выбранном дне.
     * @param day Индекс дня недели от 0 до DAYS - 1.
     * @return Количество занятых пар в выбранном дне.
     */
    int dayLoad(int day) const {
        if (day < 0 || day >= DAYS) {
            throw std::out_of_range("Invalid day index.");
        }

        int count = 0;

        for (int p = 0; p < PAIRS; p++) {
            if (_slots[day][p] != -1) {
                count++;
            }
        }

        return count;
    }
    /**
     * @brief Проверяет индексы и возвращает курс в ячейке или -1.
     * @param day Индекс дня недели от 0 до DAYS - 1.
     * @param pair Индекс пары от 0 до PAIRS - 1.
     * @return Индекс курса или -1, если пара свободна.
     */
    int getSlot(int day, int pair) const {
        if (day < 0 || day >= DAYS ||
            pair < 0 || pair >= PAIRS) {
            throw std::out_of_range("Invalid schedule slot.");
        }

        return _slots[day][pair];
    }
    /**
     * @brief Считает свободные пары между занятиями за всю неделю.
     * @return Количество свободных пар между занятиями за неделю.
     */
    int windowCount() const {
        int count = 0;
        for (int d = 0; d < DAYS; d++) {
            bool started = false;
            int gaps = 0;
            for (int p = 0; p < PAIRS; p++) {
                if (_slots[d][p] == -1) {
                    if (started) gaps++;
                }
                else {
                    count += gaps;
                    gaps = 0;
                    started = true;
                }
            }
        }
        return count;
    }
    /**
     * @brief Ищет полные допустимые расписания и сохраняет лучшее найденное.
     * @param plan Учебный план с курсами.
     * @return true, если полное допустимое расписание найдено и сохранено; иначе false.
     */
    bool generate(const StudyPlan& plan) {
        int requests[MAX_LESSONS];
        int count = 0;

        if (plan.getCount() < MIN_COURSES ||
            plan.weeklyPairs() > DAYS * MAX_DAILY_PAIRS) {
            return false;
        }

        for (int i = 0; i < plan.getCount(); i++) {
            for (int j = 0; j < plan.getCourse(i).getFrequency(); j++) {
                requests[count++] = i;
            }
        }

        // Сначала размещаем более сложные курсы.
        for (int i = 0; i < count; i++) {
            for (int j = i + 1; j < count; j++) {
                if (plan.getCourse(requests[j]).getDifficulty() >
                    plan.getCourse(requests[i]).getDifficulty()) {
                    std::swap(requests[i], requests[j]);
                }
            }
        }

        Schedule best;
        int bestScore = 1000000;

        for (int attempt = 0; attempt < SEARCH_ATTEMPTS; attempt++) {
            Schedule candidate;
            int placed = 0;

            for (int i = 0; i < count; i++) {
                const int id = requests[i];
                const int length = plan.getCourse(id).getLessonPairs();

                int possible[DAYS * PAIRS];
                int options = 0;

                for (int slot = 0; slot < DAYS * PAIRS; slot++) {
                    if (candidate.canPlace(
                        plan, id, slot / PAIRS, slot % PAIRS)) {
                        possible[options++] = slot;
                    }
                }

                if (options == 0) break;

                const int selected = possible[randomInt(0, options - 1)];

                candidate.place(
                    selected / PAIRS, selected % PAIRS, length, id);

                placed++;
            }

            if (placed == count && candidate.score(plan) < bestScore) {
                best = candidate;
                bestScore = candidate.score(plan);
            }
        }

        if (bestScore == 1000000) return false;

        *this = best;
        return true;
    }
    /**
     * @brief Оценивает влияние нагрузки, окон и времени занятия на обучение.
     * @param course Курс, используемый функцией.
     * @param day Индекс дня недели от 0 до DAYS - 1.
     * @param start Индекс первой пары занятия, начиная с 0.
     * @return Величина штрафа, используемая для уменьшения прироста знаний.
     */
    double lessonPenalty(const Course& course, int day, int start) const {
        double penalty = dayLoad(day) * 0.02 + windowCount() * 0.005;
        if (!matchesPreference(course, start)) penalty += 0.08;
        return penalty;
    }
    /**
     * @brief Выводит недельное расписание с курсами и преподавателями.
     * @param plan Учебный план с курсами.
     */
    void print(const StudyPlan& plan) const {
        std::cout << "\nWEEKLY SCHEDULE\n" << std::left
            << std::setw(6) << "Day" << std::setw(6) << "Pair"
            << std::setw(22) << "Course" << std::setw(12) << "Difficulty"
            << std::setw(12) << "Pairs/lesson" << "Teacher\n";
        for (int d = 0; d < DAYS; d++) {
            if (d > 0) std::cout << "-----------------------\n";
            for (int p = 0; p < PAIRS; p++) {
                std::cout << std::setw(6) << DAY_NAMES[d] << std::setw(6) << p + 1;
                const int id = _slots[d][p];
                if (id == -1) {
                    std::cout << "Free\n";
                    continue;
                }
                const Course& course = plan.getCourse(id);
                std::cout << std::setw(22) << course.getName()
                    << std::setw(12) << course.getDifficulty()
                    << std::setw(12) << course.getLessonPairs()
                    << TEACHER_NAMES[course.getTeacher()] << '\n';
            }
        }
        std::cout << std::right << "One pair = 2 academic hours.\n"
            << "Consecutive rows of the same course form one lesson.\n";
    }
    /**
     * @brief Выводит дневную нагрузку, количество окон и штраф расписания.
     * @param plan Учебный план с курсами.
     */
    void printStatistics(const StudyPlan& plan) const {
        std::cout << "\nDaily workloads (pairs): ";
        for (int d = 0; d < DAYS; d++) {
            std::cout << DAY_NAMES[d] << '=' << dayLoad(d) << ' ';
        }
        std::cout << "\nAverage daily workload: " << plan.weeklyPairs() / 5.0
            << "\nGaps: " << windowCount()
            << "\nPenalty (lower is better): " << score(plan) << '\n';
    }
};

class StudentGroup {
private:
    std::string _name;
    double _ability;
    int _attendance;
    double _knowledge[MAX_COURSES]{};
    int _grades[MAX_COURSES]{};
public:
    /**
     * @brief Создаёт группу с заданными подготовкой и посещаемостью.
     * @param name Название курса или группы.
     * @param ability Коэффициент подготовки группы: больше 0 и не больше 1.2.
     * @param attendance Вероятность посещения занятия в процентах, от 0 до 100.
     */
    StudentGroup(const std::string& name, double ability, int attendance)
        : _name(name), _ability(ability), _attendance(attendance) {
        if (name.empty() || ability <= 0 || ability > 1.2 || attendance < 0 || attendance > 100)
            throw std::invalid_argument("Invalid group parameters.");

        for (int i = 0; i < MAX_COURSES; i++) {
            _knowledge[i] = 10;
        }
    }
    /**
     * @brief Моделирует посещение и увеличивает знания с учётом условий обучения.
     * @param course Курс, используемый функцией.
     * @param id Индекс курса, начиная с 0.
     * @param schedulePenalty Штраф расписания, уменьшающий прирост знаний.
     */
    void attendLesson(const Course& course, int id, double schedulePenalty) {
        if (randomInt(1, 100) > _attendance) return;
        const double lessonShare = 2.0 * course.getLessonPairs() / course.getDurationHours();
        const double difficultyFactor = 1.15 - 0.05 * course.getDifficulty();
        _knowledge[id] += 100 * lessonShare * _ability * difficultyFactor * (1 - schedulePenalty);
        if (_knowledge[id] > 100) _knowledge[id] = 100;
    }
    /**
     * @brief Проводит экзамены по методикам курсов и сохраняет оценки.
     * @param plan Учебный план с курсами.
     */
    void takeExams(const StudyPlan& plan) {
        for (int i = 0; i < plan.getCount(); i++)
            _grades[i] = plan.getCourse(i).checkKnowledge(_knowledge[i]);
    }
    /**
     * @brief Возвращает экзаменационную оценку группы по индексу курса.
     * @param index Индекс курса, начиная с 0.
     * @return Сохранённая оценка по курсу; до экзамена 0.
     */
    int getGrade(int index) const {
        return _grades[index];
    }
    /**
     * @brief Выводит знания, оценки и долю сданных группой экзаменов.
     * @param plan Учебный план с курсами.
     */
    void printResults(const StudyPlan& plan) const {
        std::cout << "\nGroup " << _name << ", preparation=" << _ability
            << ", attendance probability=" << _attendance << "%\n";
        int passed = 0;
        int sum = 0;
        for (int i = 0; i < plan.getCount(); i++) {
            sum += _grades[i];
            if (_grades[i] >= 3) passed++;
            std::cout << plan.getCourse(i).getName() << ": knowledge=" << _knowledge[i]
                << ", grade=" << _grades[i] << '\n';
        }
        std::cout << "Average grade: " << static_cast<double>(sum) / plan.getCount()
            << "; passed: " << passed << '/' << plan.getCount()
            << " (" << 100.0 * passed / plan.getCount() << "%)\n";
    }
};

/**
 * @brief Ранжирует преподавателей по средним экзаменационным оценкам групп.
 * @param plan Учебный план с курсами.
 * @param groups Массив из GROUPS групп с результатами экзаменов.
 */
void printTeacherRating(const StudyPlan& plan, const StudentGroup groups[]) {
    double averages[TEACHERS]{};
    int counts[TEACHERS]{};
    for (int i = 0; i < plan.getCount(); i++) {
        const int teacher = plan.getCourse(i).getTeacher();
        for (int g = 0; g < GROUPS; g++) {
            averages[teacher] += groups[g].getGrade(i);
            counts[teacher]++;
        }
    }
    for (int t = 0; t < TEACHERS; t++) {
        averages[t] = counts[t] ? averages[t] / counts[t] : -1;
    }
    std::cout << "\nTEACHER RATING (average exam grades)\n";
    for (int rank = 0; rank < TEACHERS; rank++) {
        int best = 0;
        for (int t = 1; t < TEACHERS; t++) {
            if (averages[t] > averages[best]) best = t;
        }
        if (averages[best] < 0) break;
        std::cout << rank + 1 << ". " << TEACHER_NAMES[best] << ": "
            << averages[best] << " (exams: " << counts[best] << ")\n";
        averages[best] = -1;
    }
    std::cout << std::right;
}

/**
 * @brief Моделирует 16 недель обучения, экзамены и итоговую статистику.
 * @param plan Учебный план; счётчики проведённых часов сбрасываются и обновляются.
 * @param schedule Недельное расписание.
 */
void simulateSemester(StudyPlan& plan, const Schedule& schedule) {
    StudentGroup groups[GROUPS] = {
        StudentGroup("A", 0.95, 95), StudentGroup("B", 0.80, 90), StudentGroup("C", 0.65, 85)
    };
    plan.resetProgress();
    for (int week = 0; week < WEEKS; week++) {
        for (int d = 0; d < DAYS; d++) {
            for (int p = 0; p < PAIRS; p++) {
                const int id = schedule.getSlot(d, p);
                if (id == -1 || (p > 0 && schedule.getSlot(d, p - 1) == id)) continue;
                const Course& course = plan.getCourse(id);
                plan.recordLesson(id);
                const double penalty = schedule.lessonPenalty(course, d, p);
                for (int g = 0; g < GROUPS; g++) {
                    groups[g].attendLesson(course, id, penalty);
                }
            }
        }
    }
    for (int g = 0; g < GROUPS; g++) {
        groups[g].takeExams(plan);
        groups[g].printResults(plan);
    }
    plan.printProgress();
    printTeacherRating(plan, groups);
}

/**
 * @brief Генерирует учебный план и подбирает допустимое расписание.
 * @param plan Учебный план для заполнения случайными курсами.
 * @param schedule Расписание для заполнения занятиями созданного плана.
 */
void generatePlan(StudyPlan& plan, Schedule& schedule) {
    for (int attempt = 0; attempt < 100; attempt++) {
        plan.generate();
        if (schedule.generate(plan)) return;
    }
    throw std::runtime_error("Could not generate a feasible study plan.");
}

/**
 * @brief Проверяет ввод пункта меню и повторяет запрос при ошибке.
 * @return Пункт меню от 0 до 5; при конце ввода 0.
 */
int readChoice() {
    while (true) {
        std::cout << "\nYour choice: ";
        std::string line;
        if (!std::getline(std::cin, line)) {
            if (std::cin.eof()) return 0;
            throw std::runtime_error("Input stream error.");
        }
        std::istringstream input(line);
        int choice;
        char extra;
        if ((input >> choice) && !(input >> extra) && choice >= 0 && choice <= 5)
            return choice;
        std::cout << "Enter an integer from 0 to 5.\n";
    }
}

/**
 * @brief Создаёт объекты программы, обрабатывает меню и исключения.
 * @return 0 при успешном завершении, 1 при обработанном исключении.
 */
int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    std::cout << std::fixed << std::setprecision(2);
    try {
        StudyPlan plan;
        Schedule schedule;
        generatePlan(plan, schedule);
        while (true) {
            std::cout << "1 - Study plan\n2 - Weekly schedule\n"
                << "3 - Schedule analysis\n4 - Semester and exams\n"
                << "5 - Generate new plan and schedule\n0 - Exit\n";
            const int choice = readChoice();
            if (choice == 0) break;
            switch (choice) {
            case 1: plan.print(); break;
            case 2: schedule.print(plan); break;
            case 3: schedule.printStatistics(plan); break;
            case 4: simulateSemester(plan, schedule); break;
            case 5:
                generatePlan(plan, schedule);
                std::cout << "New study plan and schedule generated.\n";
                break;
            }
        }
    }
    catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
    return 0;
}
