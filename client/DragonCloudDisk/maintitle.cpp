#include "maintitle.h"
#include "ui_maintitle.h"
#include<QMouseEvent>
MainTitle::MainTitle(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainTitle)
{
    ui->setupUi(this);


}

void MainTitle::setParent(QWidget *parent)
{
    m_parent=parent;
}

MainTitle::~MainTitle()
{
    delete ui;
}

void MainTitle::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button()==Qt::LeftButton)
    {
        m_postion=ev->globalPos()-m_parent->geometry().topLeft();
    }
}

void MainTitle::mouseMoveEvent(QMouseEvent *ev)
{
    if(ev->buttons()&Qt::LeftButton)
    {
        QPoint pos = ev->globalPos()-m_postion;
        m_parent->move(pos);

    }
}

void MainTitle::on_mainnMinBtn_clicked()
{
    emit minWindow();
}


void MainTitle::on_mainMaxBtn_clicked()
{
    static bool isMax=false;
    if(!isMax)
    {
        ui->mainMaxBtn->setIcon(QIcon(":/images/Maximize.png"));
    }
    else
    {
        ui->mainMaxBtn->setIcon(QIcon(":/images/Maximize-1.png"));
    }
    isMax=!isMax;
    emit maximizeWindow();
}


void MainTitle::on_mainCloseBtn_clicked()
{
    emit closeWindow();
}

