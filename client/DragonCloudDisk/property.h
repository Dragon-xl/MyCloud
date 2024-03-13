#ifndef PROPERTY_H
#define PROPERTY_H

#include "common.h"
#include <QDialog>

namespace Ui {
class Property;
}

class Property : public QDialog
{
    Q_OBJECT

public:
    explicit Property(QWidget *parent = nullptr);
    ~Property();
    void setProperty(FileInfo* fileInfo);

private:
    Ui::Property *ui;
};

#endif // PROPERTY_H
