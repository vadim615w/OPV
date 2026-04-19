#include <iostream>
#include <thread>
#include <future>
#include <vector>
#include <chrono>
#include <atomic>
#include <windows.h>

using namespace std;

// Атомарный счетчик для контроля состояния 
atomic<int> completedCounter(0);

// вычисление факториала
long long computeFactorial(int n) {
    // Имитация долгой работы
    this_thread::sleep_for(chrono::milliseconds(500));
    long long res = 1;
    for (int i = 1; i <= n; ++i) {
        res *= i;
    }

    // Использование атомарного fetch_add
    completedCounter.fetch_add(1, memory_order_relaxed);
    return res;
}

// 3
void monitorSystem(int totalTasks, promise<string>&& p) {
    while (completedCounter.load() < totalTasks) {
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    // Передаем сигнал через promise
    p.set_value("Все задачи успешно выполнены монитором.");
}

int main() {
    SetConsoleOutputCP(65001);

    const int N = 5;
    vector<future<long long>> futures;
    vector<thread> threads;

    cout << "Запуск системы" << endl;

    // 1
    for (int i = 1; i <= N; ++i) {
        packaged_task<long long(int)> task(computeFactorial);

        futures.push_back(task.get_future());

        threads.push_back(thread(std::move(task), i + 10));
    }

    // 3
    promise<string> completionPromise;
    future<string> completionFuture = completionPromise.get_future();

    thread monitorThread(monitorSystem, N, std::move(completionPromise));

    // 2
    cout << "Основной поток" << endl;
    for (int i = 0; i < futures.size(); ++i) {
        // Продемонстрируем блокировку: get() ждет завершения
        long long result = futures[i].get();
        cout << "Задача [" << i << "] завершена. Результат: " << result << endl;
    }

    // Ожидаем сигнал от монитора через promise/future
    cout << "Основной поток: ожидаю финальный сигнал от монитора" << endl;
    string monitorSignal = completionFuture.get();
    cout << "Сигнал получен: " << monitorSignal << endl;

    // Завершаем потоки
    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }
    if (monitorThread.joinable()) monitorThread.join();

    cout << "Программа завершена. Всего обработано: " << completedCounter.load() << endl;

    return 0;
}