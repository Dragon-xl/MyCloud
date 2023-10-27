#ifndef FUNTION_H
#define FUNTION_H

#include <QWidget>

class Funtion : public QWidget
{
    Q_OBJECT
public:
    explicit Funtion(QWidget *parent = nullptr);
public:
    QString getCfgValue(QString name,QString key,QString pathname);

signals:

};

#endif // FUNTION_H
