#include <iostream>
#include <complex>
#include <omp.h>
#include <opencv2/opencv.hpp>
#include <windows.h>

#pragma comment(lib, "opencv_world480d.lib")

using namespace std;
using namespace cv;

int main() {
    SetConsoleOutputCP(65001);


    const int width = 1000;
    const int height = 1000;
    const int max_iter = 1000;
    const double x_min = -2.5, x_max = 1.0;
    const double y_min = -1.5, y_max = 1.5;

    Mat img(height, width, CV_8UC3);

    cout << "Расчет 1000x1000..." << endl;

    // ПОСЛЕДОВАТЕЛЬНАЯ 
    double startSeq = omp_get_wtime();
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            complex<double> c(x_min + (double)x / width * (x_max - x_min),
                y_min + (double)y / height * (y_max - y_min));
            complex<double> z(0, 0);
            int iter = 0;
            while (abs(z) <= 2.0 && iter < max_iter) {
                z = z * z + c;
                iter++;
            }
        }
    }
    double timeSeq = omp_get_wtime() - startSeq;
    cout << "Последовательно: " << timeSeq << " сек." << endl;

    // ПАРАЛЛЕЛЬНАЯ
    omp_set_num_threads(4);
    double startPar = omp_get_wtime();

#pragma omp parallel for schedule(dynamic)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            complex<double> c(x_min + (double)x / width * (x_max - x_min),
                y_min + (double)y / height * (y_max - y_min));
            complex<double> z(0, 0);
            int iter = 0;

            while (abs(z) <= 2.0 && iter < max_iter) {
                z = z * z + c;
                iter++;
            }

            if (iter == max_iter) {
                img.at<Vec3b>(y, x) = Vec3b(0, 0, 0); 
            }
            else {
                unsigned char res = (unsigned char)((iter % 64) * 4);
                img.at<Vec3b>(y, x) = Vec3b(res, res, res);
            }
        }
    }
    double timePar = omp_get_wtime() - startPar;

    cout << "Параллельно: " << timePar << " сек." << endl;
    cout << "Ускорение: " << timeSeq / timePar << " раз(а)" << endl;

    imshow("Mandelbrot Result", img);
    waitKey(0);

    return 0;
}
