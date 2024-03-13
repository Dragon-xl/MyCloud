#include "mymenu.h"
#include<QMenu>
MyMenu::MyMenu(QWidget* parent) : QMenu(parent)
{
    setStyleSheet(
        "background-color: rgba(89, 165, 245,90);"
        "color:rgb(255, 255, 0);"
        "font: 14pt \"新宋体\";"
        );
}
