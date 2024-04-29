#include "removeconfirm.h"
#include "ui_removeconfirm.h"

removeConfirm::removeConfirm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::removeConfirm)
{
    ui->setupUi(this);
    //this->setWindowFlag(Qt::FramelessWindowHint);


}

removeConfirm::~removeConfirm()
{
    delete ui;
}

void removeConfirm::on_comfirmBtn_clicked()
{
    accept();
}


void removeConfirm::on_cancelccBtn_clicked()
{
    reject();
}

