#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFileDialog>
#include "myfun.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_selectPic_pushButton_clicked();

    void on_start_pushButton_clicked();

    void on_RUOK_pushButton_clicked();

private:
    Ui::Widget *ui;
    Mat mat;
};

#endif // WIDGET_H
