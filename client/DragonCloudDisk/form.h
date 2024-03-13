#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include "common.h"
namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();
    void showRecord(QString path =RECORDFILE);
    void showUp();
    void showDown();

signals:
    void currentTabSignal(QString s);

private:
    Ui::Form *ui;
};

#endif // FORM_H
