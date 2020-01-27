#include "myfun.h"

void LabelDisplayMat(QLabel *label, Mat mat)
{
    Mat Rgb;
    QImage Img;
    if (mat.channels() == 3)//RGB Img
    {
        cvtColor(mat, Rgb, CV_BGR2RGB);//颜色空间转换
        Img = QImage((const uchar*)(Rgb.data), Rgb.cols, Rgb.rows, Rgb.cols * Rgb.channels(), QImage::Format_RGB888);
    }
    else//Gray Img
    {
        Img = QImage((const uchar*)(mat.data), mat.cols, mat.rows, mat.cols*mat.channels(), QImage::Format_Indexed8);
    }
    label->setPixmap(QPixmap::fromImage(Img));
}

double calDistance(double x1, double y1, double x2, double y2)
{
    return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}
array<double, 2> GetSrcIndex_Rotation(array<double, 6> args)
{
    double x = args[0];
    double y = args[1];
    double x_center = args[2];
    double y_center = args[3];
    double r = args[4];
    double amax = args[5];
    double d;
    double a;
    array<double, 2> res;

    d = calDistance(x, y, x_center, y_center);
    a = amax * (r-d) / r;
    res[0] = (x-x_center) * cos(a*M_PI/180) - (y-y_center)*sin(a*M_PI/180) + x_center;
    res[1] = (x-x_center) * sin(a*M_PI/180) + (y-y_center)*cos(a*M_PI/180) + y_center;
    return res;
}

array<double, 2> GetSrcIndex_ConvexLens(array<double, 6> args)
{
    double x = args[0];
    double y = args[1];
    double x_center = args[2];
    double y_center = args[3];
    double r = args[4];
    double d;
    array<double, 2> res;

    d = calDistance(x, y, x_center, y_center);
    if(d == 0)
    {
        res[0] = x_center;
        res[1] = y_center;
    }
    else
    {
        res[0] = (x-x_center) * (2*r*asin(d/r) / (d*M_PI)) + x_center;
        res[1] = (y-y_center) * (2*r*asin(d/r) / (d*M_PI)) + y_center;
    }
    return res;
}

array<double, 2> GetSrcIndex_ConcaveLens(array<double, 6> args)
{
    double x = args[0];
    double y = args[1];
    double x_center = args[2];
    double y_center = args[3];
    double r = args[4];
    double d;
    array<double, 2> res;

    d = calDistance(x, y, x_center, y_center);
    if(d == 0)
    {
        res[0] = x_center;
        res[1] = y_center;
    }
    else
    {
        res[0] = (x-x_center) * ((d*M_PI) / (2*r*asin(d/r))) + x_center;
        res[1] = (y-y_center) * ((d*M_PI) / (2*r*asin(d/r))) + y_center;
    }
    return res;
}

Mat Interpolation(Mat src, int method, array<double, 2>(*GetSrcIndex)(array<double, 6>),double r_rate, double amax)
{
    Mat dst = Mat::zeros(src.rows, src.cols, src.type());
    array<double, 2> srcIndex;
    array<double, 6> args;
    array<double, 4> weight_x;
    array<double, 4> weight_y;
    int x_center = floor((src.cols + 1)/2);
    int y_center = floor((src.rows + 1)/2);
    int x_left;
    int x_right;
    int y_up;
    int y_down;
    int dx;
    int dy;
    switch (method) {
    case 0://Nearest Neighbor interpolation
        for(int x=0; x<dst.cols; x++)
        {
            for(int y=0; y<dst.rows; y++)
            {
                args[0] = x;
                args[1] = y;
                args[2] = x_center;
                args[3] = y_center;
                args[4] = r_rate * (x_center < y_center ? x_center : y_center);
                args[5] = amax;
                if(calDistance(x, y, x_center, y_center) < args[4])
                {
                    srcIndex = GetSrcIndex(args);
                    if(srcIndex[0]<0 || srcIndex[0]>=src.cols-1 || srcIndex[1]<0 || srcIndex[1]>= src.rows-1)
                    {
                        //do nothing
                    }
                    else
                    {
                        x_left = int(srcIndex[0]);
                        y_up = int(srcIndex[1]);
                        x_right = x_left + 1;
                        y_down = y_up + 1;
                        // if left up is nearest
                        if(srcIndex[0]-x_left < 0.5 && srcIndex[1]-y_up < 0.5)
                        {
                            if (dst.channels() == 1) //single channel
                            {
                                dst.at<uchar>(y, x) = src.at<uchar>(y_up, x_left);
                            }
                            else if (dst.channels() == 3) //RGB channel
                            {
                                dst.at<Vec3b>(y, x) = src.at<Vec3b>(y_up, x_left);
                            }
                        }
                        // if right up is nearest
                        if(srcIndex[0]-x_left >= 0.5 && srcIndex[1]-y_up < 0.5)
                        {
                            if (dst.channels() == 1) //single channel
                            {
                                dst.at<uchar>(y, x) = src.at<uchar>(y_up, x_right);
                            }
                            else if (dst.channels() == 3) //RGB channel
                            {
                                dst.at<Vec3b>(y, x) = src.at<Vec3b>(y_up, x_right);
                            }
                        }
                        // if left down is nearest
                        if(srcIndex[0]-x_left < 0.5 && srcIndex[1]-y_up >= 0.5)
                        {
                            if (dst.channels() == 1) //single channel
                            {
                                dst.at<uchar>(y, x) = src.at<uchar>(y_down, x_left);
                            }
                            else if (dst.channels() == 3) //RGB channel
                            {
                                dst.at<Vec3b>(y, x) = src.at<Vec3b>(y_down, x_left);
                            }
                        }
                        // if right down is nearest
                        if(srcIndex[0]-x_left >= 0.5 && srcIndex[1]-y_up >= 0.5)
                        {
                            if (dst.channels() == 1) //single channel
                            {
                                dst.at<uchar>(y, x) = src.at<uchar>(y_down, x_right);
                            }
                            else if (dst.channels() == 3) //RGB channel
                            {
                                dst.at<Vec3b>(y, x) = src.at<Vec3b>(y_down, x_right);
                            }
                        }
                    }
                }
                else
                {
                    if (dst.channels() == 1) //single channel
                    {
                        dst.at<uchar>(y, x) = src.at<uchar>(y, x);
                    }
                    else if (dst.channels() == 3) //RGB channel
                    {
                        dst.at<Vec3b>(y, x) = src.at<Vec3b>(y, x);
                    }
                }
            }
        }
        break;
    case 1://Bilinear Intertpolatioin
        for(int x=0; x<dst.cols; x++)
        {
            for(int y=0; y<dst.rows; y++)
            {
                args[0] = x;
                args[1] = y;
                args[2] = x_center;
                args[3] = y_center;
                args[4] = r_rate * (x_center < y_center ? x_center : y_center);
                args[5] = amax;
                if(calDistance(x, y, x_center, y_center) < args[4])
                {
                    srcIndex = GetSrcIndex(args);
                    if(srcIndex[0]<0 || srcIndex[0]>=src.cols-1 || srcIndex[1]<0 || srcIndex[1]>= src.rows-1)
                    {
                        //do nothing
                    }
                    else
                    {
                        x_left = int(srcIndex[0]);
                        y_up = int(srcIndex[1]);
                        x_right = x_left + 1;
                        y_down = y_up + 1;
                        dx = srcIndex[0] - x_left;
                        dy = srcIndex[1] - y_up;
                        if (dst.channels() == 1) //single channel
                        {
                            dst.at<uchar>(y, x) = (1-dx) * (1-dy) * src.at<uchar>(y_up, x_left) + (1-dx) * (dy) * src.at<uchar>(y_down, x_left)
                                    + (dx) * (1-dy) * src.at<uchar>(y_up, x_right) + (dx) * (dy) * src.at<uchar>(y_down, x_right);
                        }
                        else if (dst.channels() == 3) //RGB channel
                        {
                            dst.at<Vec3b>(y, x) = (1-dx) * (1-dy) * src.at<Vec3b>(y_up, x_left) + (1-dx) * (dy) * src.at<Vec3b>(y_down, x_left)
                                    + (dx) * (1-dy) * src.at<Vec3b>(y_up, x_right) + (dx) * (dy) * src.at<Vec3b>(y_down, x_right);
                        }
                    }
                }
                else
                {
                    if (dst.channels() == 1) //single channel
                    {
                        dst.at<uchar>(y, x) = src.at<uchar>(y, x);
                    }
                    else if (dst.channels() == 3) //RGB channel
                    {
                        dst.at<Vec3b>(y, x) = src.at<Vec3b>(y, x);
                    }
                }
            }
        }
        break;
    case 2://Bicubic Interpolation
        for(int x=0; x<dst.cols; x++)
        {
            for(int y=0; y<dst.rows; y++)
            {
                args[0] = x;
                args[1] = y;
                args[2] = x_center;
                args[3] = y_center;
                args[4] = r_rate * (x_center < y_center ? x_center : y_center);
                args[5] = amax;
                if(calDistance(x, y, x_center, y_center) < args[4])
                {
                    srcIndex = GetSrcIndex(args);
                    if(srcIndex[0]<1 || srcIndex[0]>=src.cols-2 || srcIndex[1]<1 || srcIndex[1]>= src.rows-2)
                    {
                        //do nothing
                    }
                    else
                    {
                        x_left = int(srcIndex[0]);
                        y_up = int(srcIndex[1]);
                        x_right = x_left + 1;
                        y_down = y_up + 1;
                        dx = srcIndex[0] - x_left;
                        dy = srcIndex[1] - y_up;
                        // x <= 1
                        weight_x[1] = abs(pow(dx, 3)) - 2*abs(pow(dx, 2)) + 1;
                        weight_x[2] = abs(pow(1-dx, 3)) - 2*abs(pow(1-dx, 2)) + 1;
                        // 1 < x < 2
                        weight_x[0] = - abs(pow(1+dx, 3)) + 5*abs(pow(1+dx, 2)) - 8*abs(1+dx) + 4;
                        weight_x[3] = - abs(pow(2-dx, 3)) + 5*abs(pow(2-dx, 2)) - 8*abs(2-dx) + 4;
                        // y <= 1
                        weight_y[1] = abs(pow(dy, 3)) - 2*abs(pow(dy, 2)) + 1;
                        weight_y[2] = abs(pow(1-dy, 3)) - 2*abs(pow(1-dy, 2)) + 1;
                        // 1 < y < 2
                        weight_y[0] = - abs(pow(1+dy, 3)) + 5*abs(pow(1+dy, 2)) - 8*abs(1+dy) + 4;
                        weight_y[3] = - abs(pow(2-dy, 3)) + 5*abs(pow(2-dy, 2)) - 8*abs(2-dy) + 4;
                        if (dst.channels() == 1) //single channel
                        {
                            for(int i=0; i<4; i++) //x,col
                            {
                                for(int j=0; j<4; j++) //y,row
                                {
                                    dst.at<uchar>(y, x) += weight_x[i]*weight_y[j]*src.at<uchar>(y_up-1+j, x_left-1+i);
                                }
                            }
                        }
                        else if (dst.channels() == 3) //RGB channel
                        {
                            for(int i=0; i<4; i++) //x,col
                            {
                                for(int j=0; j<4; j++) //y,row
                                {
                                    dst.at<Vec3b>(y, x) += weight_x[i]*weight_y[j]*src.at<Vec3b>(y_up-1+j, x_left-1+i);
                                }
                            }

                        }
                    }
                }
                else
                {
                    if (dst.channels() == 1) //single channel
                    {
                        dst.at<uchar>(y, x) = src.at<uchar>(y, x);
                    }
                    else if (dst.channels() == 3) //RGB channel
                    {
                        dst.at<Vec3b>(y, x) = src.at<Vec3b>(y, x);
                    }
                }
            }
        }
        break;
    default:
        break;
    }
    return dst;
}

double U(double d)
{
    if(d == 0)
    {
        return 0;
    }
    else
    {
        return d*d*log(d*d);
    }
}

array<array<double, 71>, 71>  matrix_inverse(array<array<double, 71>, 71> src)
{
    int i,j,k;
    double max,temp;
    // 定义一个临时矩阵tmp
    array<array<double, 71>, 71> tmp;
    array<array<double, 71>, 71> dst;
    // 将src矩阵临时存放在矩阵tmp[n][n]中
    tmp = src;
    // 初始化dst矩阵为单位矩阵
    for(i=0;i<71;i++)
    {
        for(j=0;j<71;j++)
        {
            dst[i][j] = (i == j)?(double)1:0;
        }
    }
    // 进行列主消元，找到每一列的主元
    for(i=0;i<71;i++)
    {
        max = tmp[i][i];
        // 用于记录每一列中的第几个元素为主元
        k = i;
        // 寻找每一列中的主元元素
        for(j=i+1;j<71;j++)
        {
            if(fabsf(tmp[j][i]) > fabsf(max))
            {
                max = tmp[j][i];
                k = j;
            }
        }
        //cout<<"the max number is "<<max<<endl;
        // 如果主元所在的行不是第i行，则进行行交换
        if(k != i)
        {
            // 进行行交换
            for(j=0;j<71;j++)
            {
                temp = tmp[i][j];
                tmp[i][j] = tmp[k][j];
                tmp[k][j] = temp;
                // 伴随矩阵B也要进行行交换
                temp = dst[i][j];
                dst[i][j] = dst[k][j];
                dst[k][j] = temp;
            }
        }
        if(tmp[i][i] == 0)
        {
            cout<<"\nthe matrix does not exist inverse matrix\n";
            break;
        }
        // 获取列主元素
        temp = tmp[i][i];
        // 将主元所在的行进行单位化处理
        //cout<<"\nthe temp is "<<temp<<endl;
        for(j=0;j<71;j++)
        {
            tmp[i][j] = tmp[i][j]/temp;
            dst[i][j] = dst[i][j]/temp;
        }
        for(j=0;j<71;j++)
        {
            if(j != i)
            {
                temp = tmp[j][i];
                //消去该列的其他元素
                for(k=0;k<71;k++)
                {
                    tmp[j][k] = tmp[j][k] - temp*tmp[i][k];
                    dst[j][k] = dst[j][k] - temp*dst[i][k];
                }
            }
        }
    }
    return dst;
}
