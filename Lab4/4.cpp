#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>

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
            worker.join();
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

int square(int x) {
    this_thread::sleep_for(chrono::milliseconds(100));
    return x * x;
}

int main() {
    ThreadPool pool(4);
    vector<future<int>> results;

    for (int i = 1; i <= 20; ++i) {
        results.push_back(pool.enqueue(square, i));
    }
    for (size_t i = 0; i < results.size(); ++i) {
        cout << i + 1 << "^2 = " << results[i].get() << endl;
    }

    return 0;
}
