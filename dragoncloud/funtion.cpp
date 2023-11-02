#include "funtion.h"
#include<QFile>
#include<QDebug>
#include<QJsonDocument>
#include<QJsonObject>
#include<QStringList>
#include<QMap>
Funtion::Funtion(QWidget *parent)
    : QWidget{parent}
{

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
        qDebug()<<"JsonType W"
    return QString();
    }

}

QString Funtion::writeCfgFIle(QString ip, QString port, QString pathname)
{
    QMap<QString,QVariant> web_server;
    web_server.insert("ip",ip);
    web_server.insert("port",port);

    QMap<QString,QVariant> type_path;
    type_path.insert("path",pathname);

}






