#ifndef REMOVECONFIRM_H
#define REMOVECONFIRM_H

#include <QDialog>

namespace Ui {
class removeConfirm;
}

class removeConfirm : public QDialog
{
    Q_OBJECT

public:
    explicit removeConfirm(QWidget *parent = nullptr);

    ~removeConfirm();

private slots:
    void on_comfirmBtn_clicked();

    void on_cancelccBtn_clicked();

private:
    Ui::removeConfirm *ui;
};

#endif // REMOVECONFIRM_H
