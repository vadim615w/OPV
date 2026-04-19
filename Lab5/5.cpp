#include <iostream>
#include <vector>
#include <omp.h>    
#include <windows.h>

using namespace std;

int main() {
    SetConsoleOutputCP(65001);

    const int N = 10000000; 
    vector<int> arr(N, 1);  // Массив заполнен единицами


    cout << "Размер массива: " << N << endl;

    // 1. Последовательная версия
    long long sumSeq = 0;
    double startTime = omp_get_wtime(); // Замер времени через OpenMP

    for (int i = 0; i < N; i++) {
        sumSeq += arr[i];
    }

    double endTime = omp_get_wtime();
    double seqTime = endTime - startTime;
    cout << "\n[Последовательно] Сумма: " << sumSeq << endl;
    cout << "[Последовательно] Время: " << seqTime << " сек." << endl;

    // 2
    long long sumPar = 0;
    int numThreads = 4;
    omp_set_num_threads(numThreads);

    startTime = omp_get_wtime();

#pragma omp parallel for reduction(+:sumPar)
    for (int i = 0; i < N; i++) {
        sumPar += arr[i];
    }

    endTime = omp_get_wtime();
    double parTime = endTime - startTime;

    cout << "\n[Параллельно] Сумма: " << sumPar << endl;
    cout << "[Параллельно] Потоков: " << numThreads << endl;
    cout << "[Параллельно] Время: " << parTime << " сек." << endl;

    // 3. Анализ результатов
    cout << "\n Итоги " << endl;
    if (sumSeq == sumPar) {
        cout << "Результаты совпадают" << endl;
    }
    else {
        cout << "Результаты не совпадают!" << endl;
    }

    cout << "Ускорение: " << seqTime / parTime << " раз." << endl;

    return 0;
}
