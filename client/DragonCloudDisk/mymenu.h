#ifndef MYMENU_H
#define MYMENU_H

#include <QObject>
#include<QMenu>
#include<QWidget>
class MyMenu : public QMenu
{
    Q_OBJECT
public:
    explicit MyMenu(QWidget *parent = 0);

signals:

};

#endif // MYMENU_H
