#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include"logininfo.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent =0);
    void setNormalIcon();
    ~MainWindow();
    void setUserName(QString user);
    void showWindow();
    void loginOut();
    void setproBar(double num);
signals:
    void changeUser();
    void backBtnClick();

private slots:
    void on_myfileBtn_clicked();

    void on_shareBtn_clicked();

    void on_downloadBtn_clicked();

    void on_transformBtn_clicked();

    void on_folderBackBtn_clicked();

    void on_searchBtn_clicked();

    void on_toolButton_clicked();

private:
    Ui::MainWindow *ui;
    QString m_user;
    LoginInfo* loginInstance;

};
#endif // MAINWINDOW_H
