#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <chrono>
#include <string>
#include <sstream>
#include <windows.h> 

using namespace std; 

queue<int> taskQueue;
mutex mtx;
condition_variable cv;
bool isFinished = false;

// 6
recursive_mutex logMtx;

// 4
thread_local int myTaskCount = 0;


void log_internal(string msg) {
    lock_guard<recursive_mutex> lock(logMtx);
    cout << "[LOG]: " << msg << endl;
}

void log_main(string msg) {
    lock_guard<recursive_mutex> lock(logMtx);
    log_internal(msg); 
}

// 1
void producerFunc(int count) {
    for (int i = 1; i <= count; ++i) {
        {
            // 1
            lock_guard<mutex> lock(mtx);
            taskQueue.push(i);
            log_main("Производитель создал задачу: " + to_string(i));
        }
        // 2
        cv.notify_one();

        // 5
        this_thread::sleep_for(chrono::milliseconds(100));
    }

    // Завершаем работу
    {
        lock_guard<mutex> lock(mtx);
        isFinished = true;
    }
    cv.notify_all();
    log_main("Производитель закончил работу");
}

// 2 3 4 5
void consumerFunc(int id) {
    while (true) {
        // 3
        unique_lock<mutex> ul(mtx);

        // 2
        cv.wait(ul, [] { return !taskQueue.empty() || isFinished; });

        if (taskQueue.empty() && isFinished) {
            // Перед выходом выведем статистику (Пункт 4 и 5)
            ostringstream ss;
            ss << this_thread::get_id();
            log_main("Потребитель " + to_string(id) + " (ID:" + ss.str() + ") закончил. Обработал: " + to_string(myTaskCount));
            break;
        }

        int val = taskQueue.front();
        taskQueue.pop();

        // 3
        ul.unlock();

        // Имитация обработки
        int result = val * val;
        myTaskCount++; // Увеличиваем локальный счетчик потока

        log_main("Потребитель " + to_string(id) + " обработал " + to_string(val) + " -> " + to_string(result));

        // 5
        this_thread::sleep_for(chrono::milliseconds(200));
    }
}

int main() {
    SetConsoleOutputCP(65001);

    log_main("Программа запущена");

    thread prod(producerFunc, 20); 

    vector<thread> consumers;
    for (int i = 1; i <= 3; ++i) {
        consumers.push_back(thread(consumerFunc, i));
    }

    prod.join();
    for (auto& t : consumers) {
        t.join();
    }

    log_main("Все задачи обработаны");

    return 0;
}
