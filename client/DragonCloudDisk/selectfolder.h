#ifndef SELECTFOLDER_H
#define SELECTFOLDER_H

#include <QDialog>

namespace Ui {
class SelectFolder;
}

class SelectFolder : public QDialog
{
    Q_OBJECT

public:
    explicit SelectFolder(QWidget *parent = nullptr);
    ~SelectFolder();
    QString retFolderId();
    void setListWidget(QString folderName);
    void clearItem();

private:
    Ui::SelectFolder *ui;
};

#endif // SELECTFOLDER_H
