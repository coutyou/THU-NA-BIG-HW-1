#include "widget.h"
#include "ui_widget.h"
#include "ruok.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_selectPic_pushButton_clicked()
{
    QString filename;
    QFileDialog *fd = new QFileDialog;
    filename = fd->getOpenFileName(this,tr("select pic"),"","Image file(*.jpg *.png *.bmp)",0);
    if (!filename.isNull()) //用户选择了文件
    {
        Mat src;
        src = imread(filename.toStdString());
        LabelDisplayMat(ui->src_label,src);
        this->mat = src;
    }
    delete fd;
}


void Widget::on_start_pushButton_clicked()
{
    int method = ui->method_comboBox->currentIndex();
    int way = ui->way_comboBox->currentIndex();
    double r_rate = ui->r_doubleSpinBox->value();
    double amax = ui->amax_spinBox->value();
    Mat dst;
    switch (way) {
    case 0:
        dst = Interpolation(this->mat, method, GetSrcIndex_Rotation, r_rate, amax);
        break;
    case 1:
        dst = Interpolation(this->mat, method, GetSrcIndex_ConvexLens, r_rate, 0);
        break;
    case 2:
        dst = Interpolation(this->mat, method, GetSrcIndex_ConcaveLens, r_rate, 0);
        break;
    default:
        break;
    }
    LabelDisplayMat(ui->dst_label, dst);
}

void Widget::on_RUOK_pushButton_clicked()
{
    RUOK *r = new RUOK();
    r->show();
    this->close();
}
