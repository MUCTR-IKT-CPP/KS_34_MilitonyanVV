// lab8cpp.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <string>
#include <unordered_map>
//#include <cstddef>//
#include <vector>

class IPlan {
public:
    virtual int identifier() const = 0;
    virtual std::string name() const = 0;
    virtual double monthly_cost() const = 0;

    virtual ~IPlan() = default;
};

class ISubscription {
public:
    virtual int identifier() const = 0;
    virtual int user_identifier() const = 0;
    virtual int plan_identifier() const = 0;
    virtual bool activity_status() const = 0;

    virtual ~ISubscription() = default;
};

class Plan : public IPlan {
private:
    int id_;
    std::string name_;
    double monthlyCost_;

public:
    /**
     * @brief Создаёт тариф.
     * @param id ID тарифа.
     * @param name Название.
     * @param monthlyCost Месячная стоимость.
     */
    Plan(int id, std::string name, double monthlyCost)
        : id_(id), name_(name), monthlyCost_(monthlyCost) {
    }

    /**
     * @brief Получает идентификатор тарифа.
     * @return ID тарифа.
     */
    int identifier() const override {
        return id_;
    }

    /**
     * @brief Получает название тарифа.
     * @return Название.
     */
    std::string name() const override {
        return name_;
    }

    /**
     * @brief Получает стоимость тарифа.
     * @return Месячная стоимость.
     */
    double monthly_cost() const override {
        return monthlyCost_;
    }
};

class Subscription : public ISubscription {
private:
    int id_;
    int userId_;
    int planId_;
    bool active_;
public:
    /**
     * @brief Создаёт активную подписку.
     * @param id ID подписки.
     * @param userId ID пользователя.
     * @param planId ID тарифа.
     */
    Subscription(int id, int userId, int planId)
        : id_(id), userId_(userId), planId_(planId), active_(true) {
    }

    /**
     * @brief Получает идентификатор подписки.
     * @return ID подписки.
     */
    int identifier() const override {
        return id_;
    }

    /**
     * @brief Получает владельца подписки.
     * @return ID пользователя.
     */
    int user_identifier() const override {
        return userId_;
    }

    /**
     * @brief Получает текущий тариф.
     * @return ID тарифа.
     */
    int plan_identifier() const override {
        return planId_;
    }

    /**
     * @brief Проверяет активность подписки.
     * @return true — активна, false — отменена.
     */
    bool activity_status() const override {
        return active_;
    }

    /**
     * @brief Отменяет подписку без удаления.
     */
    void cancel() {
        active_ = false;
    }

    /**
     * @brief Меняет тариф подписки.
     * @param newPlanId ID нового тарифа.
     */
    void change_plan(int newPlanId) {
        planId_ = newPlanId;
    }
};

class ISubscriptionRepo {
public:
    virtual void push_subscribe(const Subscription& subscription) = 0;
    virtual bool cancel_subscription(int subscriptionId) = 0;
    virtual  std::vector<Subscription> find_all_subscribes(int userId) const = 0;
    virtual Subscription* find_by_id(int subscriptionId) = 0;//
    virtual ~ISubscriptionRepo() = default;
};

class SubscriptionRepoInMemory : public ISubscriptionRepo {
private:
    std::vector<Subscription> subscriptions_;
    std::unordered_map<int, std::vector<std::size_t>> userIndex_;//
public:
    /**
     * @brief Добавляет подписку, если её ID не занят.
     * @param subscription Подписка для сохранения.
     * @return Ничего.
     */
    void push_subscribe(const Subscription& subscription) override {
        for (const auto& existing : subscriptions_) {
            if (existing.identifier() == subscription.identifier()) {
                std::cout << "Subscription already exists\n";
                return;
            }
        }

        subscriptions_.push_back(subscription);

        int userId = subscription.user_identifier();
        std::size_t position = subscriptions_.size() - 1;

        userIndex_[userId].push_back(position);
    }

    /**
     * @brief Помечает подписку как неактивную.
     * @param subscriptionId ID подписки.
     * @return true — найдена, false — не найдена.
     */
    bool cancel_subscription(int subscriptionId) override {
        for (std::size_t i = 0; i < subscriptions_.size(); i++) {
            int existingId = subscriptions_[i].identifier();
            if (existingId == subscriptionId) {
                subscriptions_[i].cancel();
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Находит подписки пользователя, включая отменённые.
     * находим запись с подходящим ключом.
     * @param userId ID пользователя.
     * @return Вектор копий подписок; пустой, если их нет.
     */
    std::vector<Subscription> find_all_subscribes(int userId) const override {
        std::vector<Subscription> result;
        auto it = userIndex_.find(userId); //
        if (it == userIndex_.end()) {
            return result;
        }

        for (auto position : it->second) {
            result.push_back(subscriptions_[position]);
        }
        return result;

    }

    /**
     * @brief Находит подписку по ID.
     * @param subscriptionId ID подписки.
     * @return Указатель на подписку или nullptr.
     */
    Subscription* find_by_id(int subscriptionId) override {
        for (std::size_t i = 0; i < subscriptions_.size(); i++) {
            if (subscriptions_[i].identifier() == subscriptionId) {
                return &subscriptions_[i];
            }
        }
        return nullptr;
    }
};

class IPaymentGateway {//
public:
    virtual bool charge(int userId, double amount) = 0;
    virtual ~IPaymentGateway() = default;

};

class PaymentGatewayMock : public IPaymentGateway {
private:
    bool paymentSucceeds_;
public:
    /**
     * @brief Задаёт результат имитации платежей.
     * @param pay true — успех, false — отказ.
     */
    PaymentGatewayMock(bool pay)
        : paymentSucceeds_(pay) {
    }

    /**
     * @brief Имитирует списание средств.
     * @param userId ID пользователя (не используется).
     * @param amount Сумма (не используется).
     * @return Заданный результат платежа.
     */
    bool charge(int userId, double amount) override {
        return paymentSucceeds_;
    }

};

struct Transaction {
    int subscriptionId;
    int userId;
    double amount;
    bool success;
};


class BillingService {//
private:
    ISubscriptionRepo& repo_; 
    IPaymentGateway& gateway_; 
    std::vector<Transaction> transactions_;
    std::unordered_map<int, std::vector<std::size_t>> transactionIndex_;
public:
    /**
     * @brief Подключает хранилище и платёжный шлюз.
     * @param repo Ссылка на хранилище подписок.
     * @param gateway Ссылка на платёжный шлюз.
     */
    BillingService(ISubscriptionRepo& repo, IPaymentGateway& gateway)
        : repo_(repo), gateway_(gateway) {
    }

    /**
     * @brief Меняет тариф с перерасчётом и оплатой; отрицательную доплату отклоняет.
     * @param subscriptionId ID подписки.
     * @param oldPlan Старый тариф.
     * @param newPlan Новый тариф.
     * @param remainingDays Оставшиеся дни.
     * @param daysInMonth Дни в расчётном месяце.
     * @return true — тариф изменён, false — смена отклонена.
     */
    bool change_plan(int subscriptionId, const IPlan& oldPlan,
        const IPlan& newPlan, int remainingDays, int daysInMonth) {
        Subscription* subscription = repo_.find_by_id(subscriptionId);
        if (subscription == nullptr ||
            !subscription->activity_status() ||
            subscription->plan_identifier() != oldPlan.identifier() ||
            daysInMonth <= 0 ||
            remainingDays < 0 || remainingDays > daysInMonth ||
            oldPlan.monthly_cost() < 0 || newPlan.monthly_cost() < 0) {
            return false;
        }

        double amount = (newPlan.monthly_cost() - oldPlan.monthly_cost())
            * remainingDays / daysInMonth;
        if (amount < 0) {
            return false;
        }
        int userId = subscription->user_identifier();

        bool success = true;
        if (amount > 0) {
            success = gateway_.charge(userId, amount);
        }

        Transaction transaction{ subscriptionId, userId,amount, success };
        transactions_.push_back(transaction);

        std::size_t position = transactions_.size() - 1;
        transactionIndex_[userId].push_back(position);

        if (!success) {
            return false;
        }
        subscription->change_plan(newPlan.identifier());
        return true;
    }

    /**
     * @brief Находит историю операций пользователя.
     * @param userId ID пользователя.
     * @return Вектор копий операций; пустой, если их нет.
     */
    std::vector<Transaction> find_transaction(int userId) const {
        std::vector<Transaction> result;
        auto it = transactionIndex_.find(userId);
        if (it == transactionIndex_.end()) {
            return result;
        }
        for (auto position : it->second) {
            result.push_back(transactions_[position]);
        }
        return result;
    }
};

int main() {
    SubscriptionRepoInMemory repo;//

    Plan basic(1, "Basic", 600);
    Plan premium(2, "Premium", 900);
    Plan pro(3, "Pro", 1200);

    PaymentGatewayMock gateway(true); //
    BillingService billing(repo, gateway);

    Subscription sub1(101, 10, 1);
    Subscription sub2(102, 20, 2);
    Subscription sub3(103, 10, 3);
    Subscription sub4(104, 10, 1);
    Subscription sub5(105, 20, 2);
    Subscription sub6(106, 10, 2);
    Subscription sub7(107, 30, 1);

    repo.push_subscribe(sub1);
    repo.push_subscribe(sub2);
    repo.push_subscribe(sub3);
    repo.push_subscribe(sub4);
    repo.push_subscribe(sub5);
    repo.push_subscribe(sub6);
    repo.push_subscribe(sub7);

    bool changed = billing.change_plan(104, basic, premium, 10, 30);
    bool changed1 = billing.change_plan(105, premium, pro, 8, 30);
    bool changed2 = billing.change_plan(106, premium, pro, 15, 30);

    repo.cancel_subscription(101);

    auto found = repo.find_all_subscribes(10);
    auto found1 = repo.find_all_subscribes(20);

    std::cout << "Subscriptions count: " << found.size() << '\n';

    for (const auto& inf : found) {
        std::cout << "ID: " << inf.identifier()
            << " | Plan: " << inf.plan_identifier()
            << " | Active: " << inf.activity_status() << '\n';
    }

    auto history = billing.find_transaction(10);
    std::cout << "\nPayment history:\n";

    for (const auto& transaction : history) {
        std::cout << "Subscription: " << transaction.subscriptionId
            << " | Amount: " << transaction.amount
            << " | Success: " << transaction.success << '\n';
    }

    return 0;
}