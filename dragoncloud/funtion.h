#ifndef FUNTION_H
#define FUNTION_H

#include <QWidget>

#define CFG_PATH "configure/conf.json"
class Funtion : public QWidget
{
    Q_OBJECT
public:
    explicit Funtion(QWidget *parent = nullptr);
public:
    QString getCfgValue(QString name,QString key,QString pathname = CFG_PATH);
    QString writeCfgFIle(QString ip,QString port,QString pathname = CFG_PATH);

signals:

};

#endif // FUNTION_H
