#ifndef DATABASEDIALOG_H
#define DATABASEDIALOG_H

#include <QDialog>

namespace Ui {
    class DatabaseDialog;
}

class DatabaseDialog : public QDialog {
    Q_OBJECT
public:
    DatabaseDialog(QWidget *parent = 0);
    ~DatabaseDialog();
    void SetDatabaseNames(QMap<QString, QString>* names);
    QMap<QString, QString>* GetDatabaseNames();

    void SetDefaults();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DatabaseDialog *ui;

private slots:
    void on_pushButtonDefaults_clicked();
};

#endif // DATABASEDIALOG_H
