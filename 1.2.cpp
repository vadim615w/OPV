#include <iostream>
#include <thread>
#include <chrono>

void task(int id) {
    for (int i = 1; i <= 5; i++) {
        std::cout << "Thread " << id << ": " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}

int main() {
    std::thread t1(task, 1);
    std::thread t2(task, 2);

    t1.detach(); // Отсоединяем первый поток

    for (int i = 1; i <= 3; i++) {
        std::cout << "Main thread message " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
    }

    t2.join(); // Ожидаем только второй поток

    std::cout << "Main thread finished work" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1)); // Задержка перед выходом

    return 0;
}