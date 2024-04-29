#include "foldername.h"
#include "ui_foldername.h"

FolderName::FolderName(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FolderName)
{
    ui->setupUi(this);
    //this->setWindowFlag(Qt::FramelessWindowHint);
    ui->lineEdit->setText("新建文件夹");

}

QString FolderName::folderNameContent()
{
    return ui->lineEdit->text();
}


FolderName::~FolderName()
{
    delete ui;
}
