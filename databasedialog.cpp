#include "databasedialog.h"
#include "ui_databasedialog.h"
#include "confighelper.h"

DatabaseDialog::DatabaseDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DatabaseDialog)
{
    ui->setupUi(this);
}

DatabaseDialog::~DatabaseDialog()
{
    delete ui;
}

void DatabaseDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void DatabaseDialog::SetDatabaseNames(QMap<QString, QString>* names)
{
    ConfigHelper::MapToFormLayout(names, ui->formLayout);
}

QMap<QString, QString>* DatabaseDialog::GetDatabaseNames()
{
    QMap<QString, QString>* names = ConfigHelper::FormLayoutToMap(ui->formLayout);
    return names;
}

void DatabaseDialog::SetDefaults()
{
    ui->lineEditSimulatorDBName->setText("OpenSim");
    ui->lineEditAssetServiceDBName->setText("grid");
    ui->lineEditInventoryService->setText("grid");
    ui->lineEditGridService->setText("opensim");
    ui->lineEditUserService->setText("grid");
    ui->lineEditMessagingService->setText("grid");
    ui->lineEditNHibernate->setText("TaigaWebdav");
}

void DatabaseDialog::on_pushButtonDefaults_clicked()
{
    SetDefaults();
}
