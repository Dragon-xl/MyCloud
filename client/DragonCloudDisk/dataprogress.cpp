#include "dataprogress.h"
#include "ui_dataprogress.h"

DataProgress::DataProgress(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataProgress)
{

    ui->setupUi(this);
}
void DataProgress:: setFileName(QString name)
{
    ui->label->setText(name);
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(0);
}
void DataProgress::setProgress(int current,int totle)
{
    ui->progressBar->setMaximum(totle);
    ui->progressBar->setValue(current);
}
DataProgress::~DataProgress()
{
    delete ui;
}
