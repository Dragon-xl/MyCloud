#ifndef FUNTION_H
#define FUNTION_H
#include<QObject>
#include <QWidget>
#include<QNetworkAccessManager>

#define CFG_PATH "conf/cfg.json"
class Funtion : public QObject
{
    Q_OBJECT
public:
    explicit Funtion(QObject *parent = nullptr);
public:
    void setCenterWidget(QWidget* widget);
    QString getCfgValue(QString name,QString key,QString pathname = CFG_PATH);
    QString writeCfgFIle(QString ip,QString port,QString pathname = CFG_PATH);
    void writeLoginInfo(QString userName,QString passWord,bool isRemember,QString pathname = CFG_PATH);
    QString getStatusCode(QByteArray json);
    static QNetworkAccessManager* getManager();
    QString getStrMd5(QString str);

signals:
private:
    static QNetworkAccessManager* m_manager;

};

#endif // FUNTION_H
