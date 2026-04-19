#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

void printnum(int n) {
    for (int i = 1; i <= 5; ++i) {
        cout << "Thread " << n << ": " << i << endl;
        this_thread::sleep_for(chrono::milliseconds(250));
    }
    cout << "Thread " << n << " finished" << endl;
}

int main() {
    cout << "start" << endl;

    thread t1(printnum, 1);
    thread t2(printnum, 2);

    for (int i = 1; i <= 5; ++i) {
        cout << "MAIN: " << i << endl;
        this_thread::sleep_for(chrono::milliseconds(400));
    }

    t1.join();
    t2.join();

    cout << "All threads joined" << endl;

    return 0;
}