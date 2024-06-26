#include "downloadlayout.h"
#include "qwidget.h"

//静态数据成员，类中声明，类外必须定义
DownloadLayout * DownloadLayout::instance = new DownloadLayout;

//static类的析构函数在main()退出后调用
DownloadLayout::Garbo DownloadLayout::temp; //静态数据成员，类中声明，类外定义

DownloadLayout * DownloadLayout::getInstance()
{
    return instance;
}

//设置布局
void DownloadLayout::setDownloadLayout(QWidget *p)
{

    QVBoxLayout* vlayout = new QVBoxLayout;
    // 布局设置给窗口
    p->setLayout(vlayout);
    // 边界间隔
    vlayout->setContentsMargins(0, 0, 0, 0);
    m_layout = vlayout;

    vlayout->addStretch();
}

//获取布局
QLayout *DownloadLayout::getDownloadLayout()
{
    return m_layout;
}

