#ifndef LOGINTITLEWG_H
#define LOGINTITLEWG_H
#include<QPoint>
#include <QWidget>
#include<QMouseEvent>

namespace Ui {
class LoginTitleWg;
}

class LoginTitleWg : public QWidget
{
    Q_OBJECT

public:
    explicit LoginTitleWg(QWidget *parent = nullptr);
    ~LoginTitleWg();
protected:
    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);

private:
    Ui::LoginTitleWg *ui;
    QPoint m_pt;
    QWidget* m_parent;
};

#endif // LOGINTITLEWG_H
