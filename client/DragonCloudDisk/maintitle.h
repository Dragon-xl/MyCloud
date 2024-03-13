#ifndef MAINTITLE_H
#define MAINTITLE_H

#include <QWidget>
#include<QMainWindow>>
namespace Ui {
class MainTitle;
}

class MainTitle : public QWidget
{
    Q_OBJECT

public:
    explicit MainTitle(QWidget *parent =0);
    void setParent(QWidget* parent);
    ~MainTitle();
signals:
    void minWindow();
    void maximizeWindow();
    void closeWindow();
protected:
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);

private slots:
    void on_mainnMinBtn_clicked();

    void on_mainMaxBtn_clicked();

    void on_mainCloseBtn_clicked();

private:
    Ui::MainTitle *ui;
    QPoint m_postion;
    QWidget *m_parent;
};

#endif // MAINTITLE_H
