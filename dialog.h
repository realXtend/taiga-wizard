#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "wizard.h"

namespace Ui {
    class Dialog;
}

class Dialog : public QDialog {
    Q_OBJECT
public:
    Dialog(QWidget *parent = 0);
    ~Dialog();

protected:
    void changeEvent(QEvent *e);

    bool DecideStartPath();


private:
    Ui::Dialog *ui;
    Wizard m_Wizard;
    bool m_FoundTaigaDir;

private slots:
    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
};

#endif // DIALOG_H
