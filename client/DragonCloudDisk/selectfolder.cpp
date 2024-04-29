#include "selectfolder.h"
#include "ui_selectfolder.h"

SelectFolder::SelectFolder(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectFolder)
{
    ui->setupUi(this);
    setWhatsThis(nullptr);
    ui->listWidget->setViewMode(QListView::IconMode);
    ui->listWidget->setProperty("isWrapping", QVariant(true));
    ui->listWidget->setIconSize(QSize(70,95));
    ui->listWidget->setGridSize(QSize(100,1500));
    ui->listWidget->setResizeMode(QListView::Adjust);
    ui->listWidget->setSpacing(30);
    //this->setWindowFlag(Qt::FramelessWindowHint);
}

SelectFolder::~SelectFolder()
{
    delete ui;
}

QString SelectFolder::retFolderId()
{
    QList<QListWidgetItem*> list = ui->listWidget->selectedItems();
    QListWidgetItem* item = list.takeAt(0);
    QStringList Slist = item->text().split(".");
    QString ret = Slist.takeAt(0);
    return ret;
}

void SelectFolder::setListWidget(QString folderName)
{
    ui->listWidget->addItem(new QListWidgetItem(QIcon(":/images/folder.png"),folderName));
}

void SelectFolder::clearItem()
{
    ui->listWidget->clear();
}
