#include "ruok.h"
#include "ui_ruok.h"
#include "widget.h"
#include <QFile>

RUOK::RUOK(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RUOK)
{
    ui->setupUi(this);
}

RUOK::~RUOK()
{
    delete ui;
}

void RUOK::on_img1_comboBox_activated(int index)
{
    Mat img1;
    QString str1;
    QString str2;
    array<double, 2> tmp;
    img1 = imread("face-images/" + to_string(index+1) + ".jpg");
    LabelDisplayMat(ui->img1,img1);
    this->img1 = img1;
    QFile file(QString::fromStdString("face-images/" + to_string(index+1) + ".txt"));
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))  //以只读方式打开
    {
        for(int i=0; i<68; i++)
        {
            QByteArray line = file.readLine();
            QString str(line);
            QStringList list = str.split(" ");
            str1 = list.at(0);
            str2 = list.at(1);
            str2.remove("\n");
            tmp[0] = str1.toDouble();
            tmp[1] = str2.toDouble();
            this->kp_img1[i] = tmp;
        }
    }
    file.close();
}

void RUOK::on_img2_comboBox_activated(int index)
{
    Mat img2;
    QString str1;
    QString str2;
    array<double, 2> tmp;
    img2 = imread("face-images/" + to_string(index+1) + ".jpg");
    LabelDisplayMat(ui->img2,img2);
    this->img2 = img2;
    QFile file(QString::fromStdString("face-images/" + to_string(index+1) + ".txt"));
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))  //以只读方式打开
    {
        for(int i=0; i<68; i++)
        {
            QByteArray line = file.readLine();
            QString str(line);
            QStringList list = str.split(" ");
            str1 = list.at(0);
            str2 = list.at(1);
            str2.remove("\n");
            tmp[0] = str1.toDouble();
            tmp[1] = str2.toDouble();
            this->kp_img2[i] = tmp;
        }
    }
    file.close();
}

void RUOK::on_widget_pushButton_clicked()
{
    Widget *w = new Widget();
    w->show();
    this->close();
}

void RUOK::cal_kp_img3()
{
    double x_min_1, y_min_1, x_max_1, y_max_1, x_c1, y_c1;
    double x_min_2, y_min_2, x_max_2, y_max_2, x_c2, y_c2;
    x_min_1 = this->kp_img1[0][0];
    y_min_1 = this->kp_img1[0][1];
    x_max_1 = this->kp_img1[0][0];
    y_max_1 = this->kp_img1[0][1];
    x_min_2 = this->kp_img2[0][0];
    y_min_2 = this->kp_img2[0][1];
    x_max_2 = this->kp_img2[0][0];
    y_max_2 = this->kp_img2[0][1];
    for(int i=1; i<68; i++)
    {
        if(kp_img1[i][0] < x_min_1)
        {
            x_min_1 = kp_img1[i][0];
        }
        if(kp_img1[i][1] < y_min_1)
        {
            y_min_1 = kp_img1[i][1];
        }
        if(kp_img1[i][0] > x_max_1)
        {
            x_max_1 = kp_img1[i][0];
        }
        if(kp_img1[i][1] > y_max_1)
        {
            y_max_1 = kp_img1[i][1];
        }
        if(kp_img2[i][0] < x_min_2)
        {
            x_min_2 = kp_img2[i][0];
        }
        if(kp_img2[i][1] < y_min_2)
        {
            y_min_2 = kp_img2[i][1];
        }
        if(kp_img2[i][0] > x_max_2)
        {
            x_max_2 = kp_img2[i][0];
        }
        if(kp_img2[i][1] > y_max_2)
        {
            y_max_2 = kp_img2[i][1];
        }
    }
    x_c1 = (x_min_1+x_max_1)/2;
    y_c1 = (y_min_1+y_max_1)/2;
    x_c2 = (x_min_2+x_max_2)/2;
    y_c2 = (y_min_2+y_max_2)/2;
    for(int i=0; i<68; i++)
    {
        this->kp_img3[i][0] = floor(x_c1 + (this->kp_img2[i][0]-x_c2)*(x_c1-x_min_1)/(x_c2-x_min_2));
        this->kp_img3[i][1] = floor(y_c1 + (this->kp_img2[i][1]-y_c2)*(y_c1-y_min_1)/(y_c2-y_min_2));
    }
}

void RUOK::cal_phi()
{
    array<array<double, 71>, 71> L, L_inv;
    array<array<double, 2>, 71> Y;
    array<array<double, 2>, 71> res;
    //K
    double lambda = ui->lambda_horizontalSlider->value();
    for(int i=0; i<68; i++)
    {
        for(int j=0; j<68; j++)
        {
            L[i][j] = U(calDistance(this->kp_img3[i][0],this->kp_img3[i][1],this->kp_img3[j][0],this->kp_img3[j][1])) + lambda*(i==j?1:0);
        }
    }
    //P and P^T
    for(int i=0; i<68; i++)
    {
        L[i][68] = 1;
        L[i][69] = this->kp_img3[i][0];
        L[i][70] = this->kp_img3[i][1];
        L[68][i] = 1;
        L[69][i] = this->kp_img3[i][0];
        L[70][i] = this->kp_img3[i][1];
    }
    //0
    for(int i=68; i<71; i++)
    {
        for(int j=68; j<71; j++)
        {
            L[i][j] = 0;
        }
        for(int j=0; j<2; j++)
        {
            Y[i][j] = 0;
        }
    }
    //Y
    for(int i=0; i<68; i++)
    {
        Y[i] = this->kp_img1[i];
    }
    L_inv = matrix_inverse(L);
    for(int i=0; i<71; i++)
    {
        for(int j=0; j<2; j++)
        {
            res[i][j] = 0;
            for(int k=0; k<71; k++)
            {
                res[i][j] += L_inv[i][k] * Y[k][j];
            }
        }
    }
    for(int i=0; i<68; i++)
    {
        this->w[i] = res[i];
    }
    this->a1 = res[68];
    this->ax = res[69];
    this->ay = res[70];
}

array<double, 2> RUOK::GetSrcIndex_TPS(array<double, 2> dstIndex)
{
    array<double, 2> srcIndex;
    srcIndex[0] = this->a1[0] + dstIndex[0]*this->ax[0] + dstIndex[1]*this->ay[0];
    srcIndex[1] = this->a1[1] + dstIndex[0]*this->ax[1] + dstIndex[1]*this->ay[1];
    for(int i=0; i<68; i++)
    {
        srcIndex[0] += this->w[i][0]*U(calDistance(dstIndex[0], dstIndex[1], this->kp_img3[i][0], this->kp_img3[i][1]));
        srcIndex[1] += this->w[i][1]*U(calDistance(dstIndex[0], dstIndex[1], this->kp_img3[i][0], this->kp_img3[i][1]));
    }
    return srcIndex;
}

Mat RUOK::Interpolation_TPS(int method)
{
    Mat src = this->img1;
    Mat dst = Mat::zeros(src.rows, src.cols, src.type());
    array<double, 2> dstIndex;
    array<double, 2> srcIndex;
    array<double, 4> weight_x;
    array<double, 4> weight_y;
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
                dstIndex[0] = x;
                dstIndex[1] = y;
                srcIndex = GetSrcIndex_TPS(dstIndex);
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
        }
        break;
    case 1://Bilinear Intertpolatioin
        for(int x=0; x<dst.cols; x++)
        {
            for(int y=0; y<dst.rows; y++)
            {
                dstIndex[0] = x;
                dstIndex[1] = y;
                srcIndex = GetSrcIndex_TPS(dstIndex);
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
        }
        break;
    case 2://Bicubic Interpolation
        for(int x=0; x<dst.cols; x++)
        {
            for(int y=0; y<dst.rows; y++)
            {
                dstIndex[0] = x;
                dstIndex[1] = y;
                srcIndex = GetSrcIndex_TPS(dstIndex);
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
        }
        break;
    default:
        break;
    }
    return dst;
}

void RUOK::on_generate_pushButton_clicked()
{
    cal_kp_img3();
    cal_phi();
    int method = ui->method_comboBox->currentIndex();
    Mat img3;
    img3 = Interpolation_TPS(method);
    LabelDisplayMat(ui->img3, img3);
}
