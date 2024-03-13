#include "logintitle.h"
#include "ui_logintitle.h"
#include<QMouseEvent>
LoginTitle::LoginTitle(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginTitle)
{

    ui->setupUi(this);
    m_parent = parent;
}



LoginTitle::~LoginTitle()
{
    delete ui;
}

void LoginTitle::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button()==Qt::LeftButton)
    {
        m_postion = ev->globalPos()-m_parent->geometry().topLeft();
    }
}

void LoginTitle::mouseMoveEvent(QMouseEvent *ev)
{
    if(ev->buttons() == Qt::LeftButton)
    {
        QPoint curPos = ev->globalPos()-m_postion;
        m_parent->move(curPos);
    }
}

void LoginTitle::on_titleMinBtn_clicked()
{
    emit minWindow();
}


void LoginTitle::on_titleSetBtn_clicked()
{
    emit setWidget();
}


void LoginTitle::on_titleCloseBtn_clicked()
{
    emit closeWindow();
}

