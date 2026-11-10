#include <iostream>
#include <string>
#include <vector>

class IMessage {
public:
    virtual std::string type() const = 0;
    virtual ~IMessage() = default;
};

class TextMessage : public IMessage {
private:
    std::string content;

public:
    /**
     * @param text Текст, который сохраняется в сообщении.
     */
    TextMessage(const std::string& text)
        : content(text) {
    }

    /** @return Название типа сообщения — "Text". */
    std::string type() const override {
        return "Text";
    }

    /** @return Константная ссылка на текст сообщения. */
    const std::string& getContent() const {
        return content;
    }
};

class IMessageHandler {
public:
    virtual void handle(const IMessage& message) = 0;
    virtual ~IMessageHandler() = default;
};

class ConsoleLoggingPolicy {
public:
    /**
     * @param message Полученное сообщение.
     * @details Выводит "Processing message: " и тип сообщения.
     */
    void log(const IMessage& message) const {
        std::cout << "Processing message: "
            << message.type()
            << '\n';
    }
};

class SilentPolicy {
public:
    void log(const IMessage&) const {
    }
};

/** @tparam Policy Политика, определяющая поведение log(). */
template <typename Policy>
class MessageProcessor : public IMessageHandler {
private:
    Policy policy;

public:
    /** @param message Сообщение, передаваемое политике логирования. */
    void handle(const IMessage& message) override {
        policy.log(message);
    }
};

class MessageBus {
private:
    std::vector<IMessageHandler*> handlers;

public:
    /**
     * @param handler Обработчик для подписки.
     * @details Должен существовать при вызове publish().
     */
    void subscribe(IMessageHandler& handler) {
        handlers.push_back(&handler);
    }

    /** @param message Сообщение для всех подписчиков. */
    void publish(const IMessage& message) {
        for (IMessageHandler* handler : handlers) {
            handler->handle(message);
        }
    }
};

/** @tparam Derived Наследник, реализующий метод process().
 *Вызывает process() у текущего объекта-наследника.
 */
template <typename Derived>
class HandlerCRTP : public IMessageHandler {
public:
    void handle(const IMessage& message) override {
        static_cast<Derived*>(this)->process(message);
    }
};

class JsonHandler : public HandlerCRTP<JsonHandler> {
public:
    /**
     * @param message Полученное сообщение.
     * @details Выводит тип в формате JSON: { "type": "Text" }.
     */
    void process(const IMessage& message) {
        std::cout << "{ \"type\": \""
            << message.type()
            << "\" }\n";
    }
};

int main() {
    TextMessage message("My first message");

    MessageProcessor<ConsoleLoggingPolicy> consoleProcessor;
    MessageProcessor<SilentPolicy> silentProcessor;
    JsonHandler jsonHandler;

    MessageBus bus;

    bus.subscribe(consoleProcessor);
    bus.subscribe(silentProcessor);
    bus.subscribe(jsonHandler);

    bus.publish(message);

    return 0;
}