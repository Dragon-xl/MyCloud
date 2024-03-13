#include "logininfo.h"

LoginInfo* LoginInfo::m_instance = new LoginInfo;
void LoginInfo::setLoginInfo(QString user, QString ip, QString port, QString token)
{
    this->user=user;
    this->ip=ip;
    this->port=port;
    this->token=token;
}

LoginInfo *LoginInfo::getInstance()
{
    return LoginInfo::m_instance;
}

QString LoginInfo::getUser() const
{
    return user;
}

QString LoginInfo::getIp() const
{
    return ip;
}

QString LoginInfo::getPort() const
{
    return port;
}

QString LoginInfo::getToken() const
{
    return token;
}

void LoginInfo::freeInstance()
{
    if(m_instance!=NULL)
    {
        delete m_instance;
        m_instance = NULL;
    }
}

LoginInfo::LoginInfo()
{

}

LoginInfo::~LoginInfo()
{

}

LoginInfo::LoginInfo(const LoginInfo &)
{

}

LoginInfo &LoginInfo::operator=(const LoginInfo &)
{
    return *this;
}
