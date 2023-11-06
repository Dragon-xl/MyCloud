#include "funtion.h"
#include<QFile>
#include<QDebug>
#include<QJsonDocument>
#include<QJsonObject>
#include<QStringList>
#include<QMap>
#include<QJsonParseError>
#include<QApplication>
#include<QDesktopWidget>
#include<QCryptographicHash>

QNetworkAccessManager* Funtion::m_manager = new QNetworkAccessManager();
Funtion::Funtion(QObject *parent)
    : QObject{parent}
{


}

void Funtion::setCenterWidget(QWidget *widget)
{
    QDesktopWidget* desktop =  QApplication::desktop();
    widget->show();
    widget->move((desktop->width()-widget->width())/2,(desktop->height()-widget->width())/2);
}


QString Funtion::getCfgValue(QString name, QString key,QString pathname)
{
    QFile file(pathname);
    if( !file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"open "<<pathname<<" failed";
        return QString();
    }
    QByteArray json = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(json);
    if(doc.isNull()||doc.isEmpty())
    {
        qDebug()<<"doc.isNull()||doc.isEmpty()";
        return QString();
    }
    if( doc.isObject())
    {
        QJsonObject obj =  doc.object();
        QJsonObject obj1 = obj.value(name).toObject();
        QStringList list =  obj1.keys();
        for(int i =0;i<list.size();i++)
        {
            if(list.at(i) == key)
            {
                return obj1.value(list.at(i)).toString();
            }
        }
    }
    else
    {
        qDebug()<<"JsonType W";
        return QString();
    }
    return QString();
}

QString Funtion::writeCfgFIle(QString ip, QString port, QString pathname)
{
    QMap<QString,QVariant> web_server;
    web_server.insert("ip",ip);
    web_server.insert("port",port);

    QMap<QString,QVariant> type_path;
    type_path.insert("path",pathname);

}

void Funtion::writeLoginInfo(QString userName, QString passWord, bool isRemember, QString pathname)
{
    QMap<QString,QVariant> web_server;
    web_server.insert("ip",getCfgValue("web_server","ip"));
    web_server.insert("port",getCfgValue("web_server","port"));

    QMap<QString,QVariant> type_path;
    type_path.insert("path",getCfgValue("type_path","path"));



}

QString Funtion::getStatusCode(QByteArray json)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json);
    if(error.error == QJsonParseError::NoError)
    {
        if(doc.isEmpty()||doc.isNull())
        {
            qDebug()<<"doc.isEmpty()||doc.isNull()";
            return QString();
        }

    if(doc.isObject())
    {
       QJsonObject obj =  doc.object();
       QString ret = obj.value("code").toString();
       return ret;
    }
    }
    qDebug()<<error.errorString();
    return QString();

}

QNetworkAccessManager *Funtion::getManager()
{
    return m_manager;
}

QString Funtion::getStrMd5(QString str)
{
    QByteArray arr = QCryptographicHash::hash(str.toLocal8Bit(),QCryptographicHash::Md5);
    return arr.toHex();
}






