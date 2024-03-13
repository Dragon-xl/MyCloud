#ifndef LOGININFO_H
#define LOGININFO_H
#include<QString>

class LoginInfo
{
public:
    void setLoginInfo(QString user,QString ip,QString port,QString token ="");
    static LoginInfo* getInstance();
    QString getUser() const;
    QString getIp() const;
    QString getPort() const;
    QString getToken() const;

    static void freeInstance();
private:
    LoginInfo();
    LoginInfo(const LoginInfo&);
    LoginInfo& operator=(const LoginInfo&);
    ~LoginInfo();
private:
    QString user;
    QString ip;
    QString port;
    QString token;

    static LoginInfo* m_instance;
};

#endif // LOGININFO_H
