#include <iostream>
#include <cstdlib>
#include <ctime>
#include <utility>

struct TemperatureSensor {
    double temperature = 0.0;
};

struct PressureSensor {
    double pressure = 0.0;
};

struct MotionSensor {
    bool motion_detected = false;
};

enum class SensorType {
    TEMPERATURE,
    PRESSURE,
    MOTION
};

class Sensor {
private:
    void* _p_sensor_data = nullptr;
    SensorType _type = SensorType::TEMPERATURE;
    int _timestamp = 0;

public:

    Sensor() = default;

    Sensor(SensorType type, int timestamp) {
        initialize(type, timestamp);
    }

    Sensor(const Sensor& other)
        : _p_sensor_data{ other._copyData() }, _type{ other._type },
        _timestamp{ other._timestamp } {
    }

    Sensor& operator=(const Sensor& other) {
        if (this == &other) {
            return *this;
        }

        void* p_new_data = other._copyData();
        _releaseData();
        _p_sensor_data = p_new_data;
        _type = other._type;
        _timestamp = other._timestamp;
        return *this;
    }

    Sensor(Sensor&& other) noexcept
        : _p_sensor_data{ other._p_sensor_data }, _type{ other._type },
        _timestamp{ other._timestamp } {
        other._p_sensor_data = nullptr;
        other._type = SensorType::TEMPERATURE;
        other._timestamp = 0;
    }

    Sensor& operator=(Sensor&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        _releaseData();
        _p_sensor_data = other._p_sensor_data;
        _type = other._type;
        _timestamp = other._timestamp;
        other._p_sensor_data = nullptr;
        other._type = SensorType::TEMPERATURE;
        other._timestamp = 0;
        return *this;
    }

    void initialize(SensorType type, int timestamp) {
        if (_p_sensor_data != nullptr || timestamp < 0) {
            return;
        }

        switch (type) {
        case SensorType::TEMPERATURE:
            _p_sensor_data = new TemperatureSensor{};
            break;
        case SensorType::PRESSURE:
            _p_sensor_data = new PressureSensor{};
            break;
        case SensorType::MOTION:
            _p_sensor_data = new MotionSensor{};
            break;
        default:
            return;
        }

        _type = type;
        _timestamp = timestamp;
    }

    SensorType getType() const {
        return _type;
    }

    int getTimestamp() const {
        return _timestamp;
    }

    void poll() {
        if (_p_sensor_data == nullptr) {
            return;
        }

        const int MIN_TEMPERATURE_TENTHS = -500;
        const int MAX_TEMPERATURE_TENTHS = 500;
        const int MIN_PRESSURE_TENTHS = 900;
        const int MAX_PRESSURE_TENTHS = 1100;
        const double TENTHS_PER_UNIT = 10.0;

        switch (_type) {
        case SensorType::TEMPERATURE: {
            TemperatureSensor* p_sensor =
                static_cast<TemperatureSensor*>(_p_sensor_data);
            p_sensor->temperature = (MIN_TEMPERATURE_TENTHS +
                std::rand() % (MAX_TEMPERATURE_TENTHS - MIN_TEMPERATURE_TENTHS + 1))
                / TENTHS_PER_UNIT;
            break;
        }
        case SensorType::PRESSURE: {
            PressureSensor* p_sensor =
                static_cast<PressureSensor*>(_p_sensor_data);
            p_sensor->pressure = (MIN_PRESSURE_TENTHS +
                std::rand() % (MAX_PRESSURE_TENTHS - MIN_PRESSURE_TENTHS + 1))
                / TENTHS_PER_UNIT;
            break;
        }
        case SensorType::MOTION: {
            MotionSensor* p_sensor =
                static_cast<MotionSensor*>(_p_sensor_data);
            p_sensor->motion_detected = (std::rand() % 2 == 1);
            break;
        }
        }
    }

    void calibrate() {
        if (_p_sensor_data == nullptr) {
            return;
        }

        switch (_type) {
        case SensorType::TEMPERATURE:
            static_cast<TemperatureSensor*>(_p_sensor_data)->temperature = 0.0;
            break;
        case SensorType::PRESSURE:
            static_cast<PressureSensor*>(_p_sensor_data)->pressure = 0.0;
            break;
        case SensorType::MOTION:
            static_cast<MotionSensor*>(_p_sensor_data)->motion_detected = false;
            break;
        }
    }

    void print() const {
        if (_p_sensor_data == nullptr) {
            std::cout << "Sensor is not initialized.\n";
            return;
        }

        std::cout << "Creation time: " << _timestamp << " s | ";

        switch (_type) {
        case SensorType::TEMPERATURE: {
            const TemperatureSensor* p_sensor =
                static_cast<const TemperatureSensor*>(_p_sensor_data);
            std::cout << "Temperature: " << p_sensor->temperature << " C\n";
            break;
        }
        case SensorType::PRESSURE: {
            const PressureSensor* p_sensor =
                static_cast<const PressureSensor*>(_p_sensor_data);
            std::cout << "Pressure: " << p_sensor->pressure << " kPa\n";
            break;
        }
        case SensorType::MOTION: {
            const MotionSensor* p_sensor =
                static_cast<const MotionSensor*>(_p_sensor_data);
            std::cout << "Motion: ";
            if (p_sensor->motion_detected) {
                std::cout << "detected\n";
            }
            else {
                std::cout << "not detected\n";
            }
            break;
        }
        }
    }

    ~Sensor() {
        _releaseData();
    }

private:

    void* _copyData() const {
        if (_p_sensor_data == nullptr) {
            return nullptr;
        }
        switch (_type) {
        case SensorType::TEMPERATURE:
            return new TemperatureSensor{
                *static_cast<const TemperatureSensor*>(_p_sensor_data) };
        case SensorType::PRESSURE:
            return new PressureSensor{
                *static_cast<const PressureSensor*>(_p_sensor_data) };
        case SensorType::MOTION:
            return new MotionSensor{
                *static_cast<const MotionSensor*>(_p_sensor_data) };
        }
        return nullptr;
    }

    void _releaseData() noexcept {

        switch (_type) {
        case SensorType::TEMPERATURE:
            delete static_cast<TemperatureSensor*>(_p_sensor_data);
            break;
        case SensorType::PRESSURE:
            delete static_cast<PressureSensor*>(_p_sensor_data);
            break;
        case SensorType::MOTION:
            delete static_cast<MotionSensor*>(_p_sensor_data);
            break;
        }
        _p_sensor_data = nullptr;
    }
};

class SensorNetwork {
private:
    Sensor* _p_sensors = nullptr;
    int _capacity = 0;
    int _count = 0;

public:

    SensorNetwork() = default;

    explicit SensorNetwork(int capacity)
        : _p_sensors{ new Sensor[capacity > 0 ? capacity : 0] },
        _capacity{ capacity > 0 ? capacity : 0 }, _count{ 0 } {
    }

    SensorNetwork(const SensorNetwork& other) : SensorNetwork(other._capacity) {

        for (int i = 0; i < other._count; i++) {
            _p_sensors[i] = other._p_sensors[i];
        }
        _count = other._count;
    }

    SensorNetwork& operator=(const SensorNetwork& other) {
        if (this == &other) {
            return *this;
        }
        SensorNetwork copy(other);
        *this = std::move(copy);
        return *this;
    }

    SensorNetwork(SensorNetwork&& other) noexcept
        : _p_sensors{ other._p_sensors }, _capacity{ other._capacity },
        _count{ other._count } {
        other._p_sensors = nullptr;
        other._capacity = 0;
        other._count = 0;
    }

    SensorNetwork& operator=(SensorNetwork&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        delete[] _p_sensors;
        _p_sensors = other._p_sensors;
        _capacity = other._capacity;
        _count = other._count;
        other._p_sensors = nullptr;
        other._capacity = 0;
        other._count = 0;
        return *this;
    }

    bool addSensor(SensorType type, int timestamp) {
        if (_count == _capacity || timestamp < 0) {
            return false;
        }
        if (type != SensorType::TEMPERATURE &&
            type != SensorType::PRESSURE &&
            type != SensorType::MOTION) {
            return false;
        }

        _p_sensors[_count] = Sensor(type, timestamp);
        _count++;
        return true;
    }

    void pollSensors() {
        for (int i = 0; i < _count; i++) {
            _p_sensors[i].poll();
        }
    }

    void filterByType(SensorType type) const {
        bool found = false;
        for (int i = 0; i < _count; i++) {
            if (_p_sensors[i].getType() == type) {
                std::cout << "Sensor " << i + 1 << " | ";
                _p_sensors[i].print();
                found = true;
            }
        }
        if (!found) {
            std::cout << "No sensors of this type.\n";
        }
    }

    void calibrateSensors() {
        for (int i = 0; i < _count; i++) {
            _p_sensors[i].calibrate();
        }
    }

    void logValues() const {
        if (_count == 0) {
            std::cout << "Network is empty. Add sensors first.\n";
            return;
        }
        for (int i = 0; i < _count; i++) {
            std::cout << "Sensor " << i + 1 << " | ";
            _p_sensors[i].print();
        }
    }

    void findByTimestamp(int timestamp) const {
        bool found = false;
        for (int i = 0; i < _count; i++) {
            if (_p_sensors[i].getTimestamp() == timestamp) {
                std::cout << "Sensor " << i + 1 << " | ";
                _p_sensors[i].print();
                found = true;
            }
        }
        if (!found) {
            std::cout << "No sensors with this creation time.\n";
        }
    }

    ~SensorNetwork() {
        delete[] _p_sensors;
    }
};

int main() {
    const std::time_t START_TIME = std::time(nullptr);
    std::srand(static_cast<unsigned int>(START_TIME));
    const int MAX_CAPACITY = 100;
    int capacity = 0;

    std::cout << "Maximum number of sensors (1..100): ";
    if (!(std::cin >> capacity)) {
        std::cout << "Error: an integer is required.\n";
        return 1;
    }
    if (capacity < 1 || capacity > MAX_CAPACITY) {
        std::cout << "Invalid sensor capacity.\n";
        return 1;
    }

    SensorNetwork network(capacity);
    int choice = -1;

    do {
        std::cout << "\n1. Add sensor\n"
            << "2. Poll all sensors\n"
            << "3. Filter by type\n"
            << "4. Calibrate all sensors\n"
            << "5. Show log\n"
            << "6. Find by creation time\n"
            << "0. Exit\nYour choice: ";

        if (!(std::cin >> choice)) {
            std::cout << "Input error. Exiting program.\n";
            return 1;
        }

        switch (choice) {
        case 1:
        case 3: {
            int type_number = 0;
            std::cout << "Type: 1 - temperature, 2 - pressure, 3 - motion: ";
            if (!(std::cin >> type_number)) {
                std::cout << "Input error.\n";
                return 1;
            }
            if (type_number < 1 || type_number > 3) {
                std::cout << "Invalid sensor type.\n";
                break;
            }
            const SensorType TYPE = static_cast<SensorType>(type_number - 1);
            if (choice == 1) {

                const int TIMESTAMP = static_cast<int>(std::time(nullptr) - START_TIME);
                if (network.addSensor(TYPE, TIMESTAMP)) {
                    std::cout << "Sensor created. Time: " << TIMESTAMP << " s.\n";
                }
                else {
                    std::cout << "Sensor was not added: check network capacity.\n";
                }
            }
            else {
                network.filterByType(TYPE);
            }
            break;
        }
        case 2:
            network.pollSensors();
            network.logValues();
            break;
        case 4:
            network.calibrateSensors();
            network.logValues();
            break;
        case 5:
            network.logValues();
            break;
        case 6: {
            int timestamp = 0;
            std::cout << "Creation time from the log (seconds since startup): ";
            if (!(std::cin >> timestamp)) {
                std::cout << "Input error.\n";
                return 1;
            }
            network.findByTimestamp(timestamp);
            break;
        }
        case 0:
            std::cout << "Exiting program.\n";
            break;
        default:
            std::cout << "Invalid menu option.\n";
            break;
        }
    } while (choice != 0);

    return 0;
}
