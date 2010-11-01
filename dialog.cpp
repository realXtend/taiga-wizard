#include <QDir>
#include <QFile>
#include <QMessageBox>
#include "dialog.h"
#include "ui_dialog.h"
#include "wizard.h"
#include "configinifile.h"
#include "configxmlfile.h"
#include "confighelper.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    m_FoundTaigaDir = DecideStartPath();
    m_Wizard.m_RegionWizard = new RegionWizard();

    m_Wizard.m_RegionWizard->m_Wizard = &m_Wizard;
}

Dialog::~Dialog()
{
    // del widgets here    
    delete ui;
}

void Dialog::changeEvent(QEvent *e)
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



bool Dialog::DecideStartPath()
{
    QString sep = QDir::separator();
    QString current = QDir::current().absolutePath();
    QString qdirSep = "";
    current.contains("/") ? qdirSep = "/" : qdirSep = "\"";

    QString cur = QDir::current().absolutePath();
    QStringList s = cur.split(qdirSep);
    s.removeLast();
    s.append("OpenSim");
    s.append("OpenSim.exe");

    QString releasePath = s.join(qdirSep);
    QFile *r = new QFile(releasePath);
    if(r->exists())
    {
        s.removeLast();
        //ui->lineEditTaigaDir->setText(s.join(sep));
        this->m_Wizard.SetTaigaPath(s.join(sep));
        return true;
    }


    QString buildPath = ""; // figure out if we're running application from debug folder
    // figure out buildPath abs path, (dont know any smarter way to do this)
    QStringList split = current.split(qdirSep);
    if(split.count()>2) {
        split.removeLast(); // 2 folders up
        split.removeLast();
        buildPath = split.join(sep);
        buildPath.append(sep + "bin" + sep + "OpenSim.exe");
    }

    //QDir buildDir(buildPath);
    QString binPath = current + "/OpenSim.exe";
    binPath = binPath.replace("/",sep);

    QFile *file = new QFile(buildPath);
    if(file->exists()){
        QStringList split2 = buildPath.split(sep);
        split2.removeLast();
        buildPath = split2.join(sep);
        this->m_Wizard.SetTaigaPath(buildPath);
        return true;
    }
    file = new QFile(binPath);
    if(file->exists()){
        QStringList split2 = binPath.split(sep);
        split2.removeLast();
        binPath = split2.join(sep);
        this->m_Wizard.SetTaigaPath(s.join(sep));
        return true;
    }
    return false;
}

void Dialog::on_pushButton_clicked() //Region
{
    m_Wizard.m_GridConfig = false;
    m_Wizard.m_JoinGrid = false;
    m_Wizard.m_Regions = true;
    this->hide();
    m_Wizard.Configure(m_FoundTaigaDir );
    m_Wizard.show();
}

void Dialog::on_pushButton_2_clicked() //Grid
{
    m_Wizard.m_GridConfig = true;
    m_Wizard.m_JoinGrid = true;
    m_Wizard.m_Regions = true;
    this->hide();
    m_Wizard.Configure(m_FoundTaigaDir );
    m_Wizard.show();
}

void Dialog::on_pushButton_3_clicked() //Advanced
{
    QDir cwd = QDir::current();
    QString saveDir = ConfigHandler::SAVE_DIR.replace("/", QDir::separator());
    QString location = ConfigHelper::SelectPathOrFolder(QString("Select config file"), this, cwd.absolutePath() + QDir::separator() + saveDir, openFile);
    if(location!="")
    {
        // we need relative path
        QString currentPath = cwd.absolutePath();
        if(location.startsWith(currentPath))
        {
            location = location.mid(currentPath.length()+1);
        }
        else
        {
            location = cwd.relativeFilePath(location);
        }
        ConfigIniFile* loadedConf = new ConfigIniFile(location);

        //ui->listWidgetRegions->clear();
        m_Wizard.ClearRegions();

        if(loadedConf->m_Sections.contains("ConfigData"))
        {
            QMap<QString,QString>* confData = loadedConf->m_Sections.value("ConfigData");
            m_Wizard.m_GridConfig = confData->contains("grid");
            m_Wizard.m_JoinGrid = confData->contains("sim"),
            m_Wizard.m_Regions = confData->contains("regions");
        }

        ConfigHelper::SetConfigValuesToPage(loadedConf->m_Sections, &m_Wizard);
        m_Wizard.SetDataBaseParams();

        this->hide();
        m_Wizard.Configure(m_FoundTaigaDir );
        m_Wizard.show();
    }
}

