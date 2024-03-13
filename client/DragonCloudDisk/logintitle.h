#ifndef LOGINTITLE_H
#define LOGINTITLE_H

#include <QWidget>
#include<QPoint>
#include<QMainWindow>
namespace Ui {
class LoginTitle;
}

class LoginTitle : public QWidget
{
    Q_OBJECT

public:
    explicit LoginTitle(QWidget *parent=0);

    ~LoginTitle();
protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
signals:
    void closeWindow();
    void minWindow();
    void setWidget();
private slots:
    void on_titleMinBtn_clicked();

    void on_titleSetBtn_clicked();

    void on_titleCloseBtn_clicked();

private:
    Ui::LoginTitle *ui;
    QPoint m_postion;
    QWidget *m_parent;
};

#endif // LOGINTITLE_H
