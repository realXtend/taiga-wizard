#include "regionwizard.h"
#include "ui_regionwizard.h"
#include <QUuid>

RegionWizard::RegionWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::RegionWizard)
{
    ui->setupUi(this);
}

RegionWizard::~RegionWizard()
{
    delete ui;
}

void RegionWizard::changeEvent(QEvent *e)
{
    QWizard::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QGridLayout* RegionWizard::GetRegionGridLayout()
{
    return ui->formLayoutRegionData;
}
QFormLayout* RegionWizard::GetAvatarLayout()
{
    return ui->formLayoutAvatarData;
}

void RegionWizard::on_btnNewUUID_clicked()
{
    QUuid quuid = QUuid::createUuid();
    QString str = quuid.toString();
    QString plainUuid = str.mid(1,str.length()-2);
    ui->lineEdit_12->setText(plainUuid);
}

void RegionWizard::GoToStart()
{
    this->restart();    
    SetDefaults();
}

void RegionWizard::SetDefaults()
{
    this->ui->lineEditExternalHostName->setText("SYSTEMIP");
}

void RegionWizard::on_btnNextPort_clicked()
{
    QList<int> ports = this->m_Wizard->GetRegionPorts();
    int nextPort = 9000;
    bool conflict = true;
    while(nextPort<50000&&conflict==true)
    {
        nextPort++;
        bool match = false;
        foreach(int p, ports)
        {
            if(nextPort==p)
                match = true;
        }
        if(match==false)
            conflict = false;
    }
    QString strPort = QString::number(nextPort);
    ui->lineEdit_4->setText(strPort);
}
