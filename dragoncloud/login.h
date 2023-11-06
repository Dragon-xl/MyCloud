#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include"funtion.h"
namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();
    void saveWebInfo(QString ip,QString port,QString path);
    QByteArray setRegJson(QString userName,QString nickName,QString passWord,QString phone,QString email );
    QByteArray setLoginJson(QString userName ,QString passWord);
    QStringList getLoginStatus(QByteArray json);


protected:
    void paintEvent(QPaintEvent *ev);

private slots:
    void on_toolButton_3_clicked();

    void on_toolButton_reg_2_clicked();
    //void on_toolButton_2_clicked();

    void on_toolButton_lgin_clicked();

private:
    Funtion m_fun;

private:
    Ui::Login *ui;
};

#endif // LOGIN_H
