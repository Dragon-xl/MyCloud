#ifndef FOLDERNAME_H
#define FOLDERNAME_H

#include <QDialog>

namespace Ui {
class FolderName;
}

class FolderName : public QDialog
{
    Q_OBJECT

public:
    explicit FolderName(QWidget *parent =0);
    QString folderNameContent();

    ~FolderName();
signals:
//    void folderComfirm();
//    void folderCancel();
private:
    Ui::FolderName *ui;
};

#endif // FOLDERNAME_H
