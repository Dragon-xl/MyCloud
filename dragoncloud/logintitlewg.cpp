#include "logintitlewg.h"
#include "ui_logintitlewg.h"

LoginTitleWg::LoginTitleWg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginTitleWg)
{
    ui->setupUi(this);
    ui->label_icon->setPixmap(QPixmap("://images/cloud.png").scaled(40,40));
    m_parent=parent;

}

LoginTitleWg::~LoginTitleWg()
{
    delete ui;
}

void LoginTitleWg::mouseMoveEvent(QMouseEvent *ev)
{
    if(ev->buttons() & Qt::LeftButton)
    {
        m_parent->move(ev->globalPos()-m_pt);
    }
}

void LoginTitleWg::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::LeftButton)
    {
        m_pt = ev->globalPos()-m_parent->geometry().topLeft();
    }
}
