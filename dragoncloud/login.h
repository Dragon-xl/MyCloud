#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>

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
    QByteArray getRegJson(QString userName,QString nickName,QString passWord,QString phone,QString email );
protected:
    void paintEvent(QPaintEvent *ev);

private slots:
    void on_toolButton_3_clicked();

    void on_toolButton_reg_2_clicked();

private:
    Ui::Login *ui;
};

#endif // LOGIN_H
