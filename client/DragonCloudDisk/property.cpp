#include "property.h"
#include "ui_property.h"

Property::Property(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Property)
{
    ui->setupUi(this);
    ui->url->setOpenExternalLinks(true);
}

Property::~Property()
{
    delete ui;
}

void Property::setProperty(FileInfo *fileInfo)
{
    ui->filename->setText(fileInfo->filename);
    ui->user->setText(fileInfo->user);
    ui->time->setText(fileInfo->time);
    ui->pv->setText(QString("%1").arg(fileInfo->pv));
    QString tmp = QString("<a href=\"%1\">%2</a>").arg(fileInfo->url).arg(fileInfo->url);
    ui->url->setText(tmp);
    int size = fileInfo->size;
    if(size>0 && size<1024)
    {
        ui->size->setText(QString("%1B").arg(size));
    }
    else if(size>=1024 && size <1024*1024)
    {
        ui->size->setText(QString("%1KB").arg(size));
    }
    else
    {
        ui->size->setText(QString("%1MB").arg(size));
    }
    if(fileInfo->shareStatus==0)
    {
        ui->share->setText("未分享");
    }
    else
    {
        ui->share->setText("已分享");
    }
}
