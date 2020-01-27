#ifndef MYFUN_H
#define MYFUN_H

#define _USE_MATH_DEFINES
#include <QLabel>
#include <opencv2/opencv.hpp>
#include <math.h>

using namespace cv;
using namespace std;

void LabelDisplayMat(QLabel *label, Mat mat);

Mat Interpolation(Mat src, int method, array<double, 2>(*GetSrcIndex)(array<double, 6> args), double r_rate, double amax);

array<double, 2> GetSrcIndex_Rotation(array<double, 6> args);

array<double, 2> GetSrcIndex_ConvexLens(array<double, 6> args);//only 5 needed

array<double, 2> GetSrcIndex_ConcaveLens(array<double, 6> args);//only 5 needed

double calDistance(double x1, double y1, double x2, double y2);

double U(double distance);

array<array<double, 71>, 71>  matrix_inverse(array<array<double, 71>, 71> src);
#endif // MYFUN_H
