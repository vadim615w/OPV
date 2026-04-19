#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <chrono>
#include <windows.h>

using namespace std;

class ThreadPool {
private:
    vector<thread> workers;
    queue<function<void()>> tasks;
    mutex mtx;
    condition_variable cv;
    bool stop;

public:
    ThreadPool(size_t num_threads) : stop(false) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    function<void()> task;
                    {
                        unique_lock<mutex> lock(mtx);
                        cv.wait(lock, [this] {
                            return stop || !tasks.empty();
                            });

                        if (stop && tasks.empty()) return;

                        task = move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
                });
        }
    }

    ~ThreadPool() {
        {
            unique_lock<mutex> lock(mtx);
            stop = true;
        }
        cv.notify_all();
        for (thread& worker : workers) {
            if (worker.joinable()) worker.join();
        }
    }

    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> future<decltype(f(args...))> {
        using return_type = decltype(f(args...));
        auto task = make_shared<packaged_task<return_type()>>(
            bind(forward<F>(f), forward<Args>(args)...)
        );
        future<return_type> result = task->get_future();
        {
            unique_lock<mutex> lock(mtx);
            if (stop) throw runtime_error("enqueue on stopped ThreadPool");
            tasks.emplace([task]() { (*task)(); });
        }
        cv.notify_one();
        return result;
    }
};

long long factorial(int n) {
    long long res = 1;
    for (int i = 1; i <= n; i++) {
        res *= i;
    }

    this_thread::sleep_for(chrono::milliseconds(50));
    return res;
}

int main() {
    SetConsoleOutputCP(65001); 


    ThreadPool pool(4);

 
    vector<future<long long>> results;

    cout << "Пул запущен Добавляем 20 задач на факториал" << endl;


    for (int i = 1; i <= 20; ++i) {
        results.push_back(pool.enqueue(factorial, i));
    }

    cout << "Результаты вычислений" << endl;
    for (size_t i = 0; i < results.size(); ++i) {
        cout << i + 1 << "! = " << results[i].get() << endl;
    }

    cout << "Все задачи выполнены. Программа завершена" << endl;

    return 0;
}