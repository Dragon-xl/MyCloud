#ifndef UPLOADLAYOUT_H
#define UPLOADLAYOUT_H

#include<QWidget>
class UploadLayout
{
public:

    static UploadLayout* getInstance();
    void setUploadLayout(QWidget* w);
    static void freeInstance();
    QLayout* getLayout();
private:
    UploadLayout();
    UploadLayout(const UploadLayout&)=delete;
    UploadLayout& operator=(const UploadLayout&)=delete;
    ~UploadLayout();
private:
    static UploadLayout* instance;
    QWidget* m_widget;
    QLayout* m_layout;
};

#endif // UPLOADLAYOUT_H
