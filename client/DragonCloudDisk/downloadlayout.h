#ifndef DOWNLOADLAYOUT_H
#define DOWNLOADLAYOUT_H


#include "qdebug.h"
#include <QVBoxLayout>

//下载进度布局类，单例模式
class DownloadLayout
{
public:
    static DownloadLayout *getInstance(); //保证唯一一个实例
    void setDownloadLayout(QWidget *p); //设置布局
    QLayout *getDownloadLayout(); //获取布局

private:
    DownloadLayout()
    {

    }

    ~DownloadLayout()    //析构函数为私有
    {
    }

    //静态数据成员，类中声明，类外必须定义
    static DownloadLayout *instance;
    QLayout *m_layout;
    QWidget* m_wg;


    class Garbo
    {
    public:
        ~Garbo()
        {
          if(NULL != DownloadLayout::instance)
          {
            delete DownloadLayout::instance;
            DownloadLayout::instance = NULL;
            qDebug()<< "instance is detele";
          }
        }
    };

    static Garbo temp;
};


#endif // DOWNLOADLAYOUT_H
