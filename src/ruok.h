#ifndef RUOK_H
#define RUOK_H

#include <QWidget>
#include <QFileDialog>
#include "myfun.h"

namespace Ui {
class RUOK;
}

class RUOK : public QWidget
{
    Q_OBJECT

public:
    explicit RUOK(QWidget *parent = 0);
    ~RUOK();

private slots:
    void on_widget_pushButton_clicked();

    void on_generate_pushButton_clicked();

    void on_img1_comboBox_activated(int index);

    void on_img2_comboBox_activated(int index);

    void cal_kp_img3();

    void cal_phi();

    array<double, 2> GetSrcIndex_TPS(array<double, 2> dstIndex);

    Mat Interpolation_TPS(int method);

private:
    Ui::RUOK *ui;
    Mat img1;
    Mat img2;
    array<array<double, 2>, 68> kp_img1;
    array<array<double, 2>, 68> kp_img2;
    array<array<double, 2>, 68> kp_img3;
    array<double, 2> a1;
    array<double, 2> ax;
    array<double, 2> ay;
    array<array<double, 2>, 68> w;
};

#endif // RUOK_H
