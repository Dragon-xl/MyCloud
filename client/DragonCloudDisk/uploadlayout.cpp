#include "uploadlayout.h"
#include<QLayout>
#include<QVBoxLayout>
UploadLayout* UploadLayout::instance = new UploadLayout;
UploadLayout::UploadLayout()
{

}

UploadLayout* UploadLayout::getInstance()
{
    return instance;
}

void UploadLayout::freeInstance()
{
    if(instance!=nullptr)
    {
        delete instance;
        instance=nullptr;
    }
}
UploadLayout::~UploadLayout()
{

}
QLayout *UploadLayout::getLayout()
{
    return m_layout;
}
void UploadLayout::setUploadLayout(QWidget* w)
{

    QVBoxLayout* vlayout = new QVBoxLayout;
    // 布局设置给窗口
    w->setLayout(vlayout);
    // 边界间隔
    vlayout->setContentsMargins(0, 0, 0, 0);
    m_layout = vlayout;

    // 添加弹簧
    vlayout->addStretch();


}
