#include "wizard.h"
#include "ui_wizard.h"
#include "confighelper.h"
#include <QDir>
#include <QUuid>
#include "configfile.h"
#include <databasedialog.h>
#include <QMessageBox>
#include "connectionstringhelper.h"
#include <QFileDialog>


Wizard::Wizard(QWidget *parent, RegionWizard* regionWizard) :
    QWizard(parent),
    ui(new Ui::Wizard),
    m_RegionWizard(regionWizard)
{
    ui->setupUi(this);
    m_lastId = 0;
    m_missingUserName = 0;
    QAbstractButton* nextButton = this->button(QWizard::NextButton);
    connect(nextButton, SIGNAL(clicked()), SLOT(onNext_button_clicked()));
    QAbstractButton* finishButton = this->button(QWizard::FinishButton);
    connect(finishButton, SIGNAL(clicked()), SLOT(onFinish_button_clicked()));

    m_GridConfig = true;
    m_JoinGrid = true;


    // hide DB label strings
    ui->labelSimDBString->hide();
    ui->labelGridDBString->hide();
    ui->labelGridDbPlugin->hide();
    ui->labelSimDBPlugin->hide();
    ui->labelGridNhib->hide();
    ui->labelPropProv->hide();
    ui->labelModRexConnStr->hide();

    loadDefaults();
    setDatabasesFromConfig();
}

Wizard::~Wizard()
{
    if(m_missingUserName!=0)
    {
        delete m_missingUserName;
        m_missingUserName = 0;
    }
    delete ui;
}

void Wizard::SetTaigaPath(QString path)
{
    ui->lineEditTaigaDir->setText(path);
}

void Wizard::changeEvent(QEvent *e)
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


void Wizard::Configure(bool foundTaiga)
{
    if(!m_GridConfig)
    {
        this->removePage(3);
        this->removePage(1);
    }

    if(!foundTaiga)
    {
        QString text = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">";
                text.append("<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">");
        text.append("p, li { white-space: pre-wrap; }");
                text.append("</style></head><body style=\" font-family:'SansSerif'; font-size:8.25pt; font-weight:400; font-style:normal;\">");
                text.append("<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;");
                text.append("<\"-qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">");
                text.append("We will need to know where to place the conf files. <br />");
                text.append("<Please select bin folder of your Taiga instal. <br />");
                text.append("This folder will contain the Taiga executables. <br /> Such as OpenSim.exe, OpenSim.Server.exe.</span></p></body></html>");

        ui->lblLocateDirHelp->clear();
        ui->lblLocateDirHelp->setText(text);
    }
    else if(foundTaiga)
    {
        QString text = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">";
                text.append("<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">");
        text.append("p, li { white-space: pre-wrap; }");
                text.append("</style></head><body style=\" font-family:'SansSerif'; font-size:8.25pt; font-weight:400; font-style:normal;\">");
                text.append("<p align=\"center\" style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;");
                text.append("<\"-qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:10pt;\">");
                text.append("We have found Taiga directory. <br />");
                text.append("If you agree this is right directory, continue by clicking next. <br />");
                text.append("If not, please select the right location, that contains Taiga executables, <br /> such as OpenSim.exe, OpenSim.Server.exe.</span></p></body></html>");

        ui->lblLocateDirHelp->clear();
        ui->lblLocateDirHelp->setText(text);
    }
    setWritableFiles();
}

void Wizard::onNext_button_clicked()
{
    int id = this->currentId();

    if(m_GridConfig){
        switch (id)
        {
        case 1:
            if(!checkTaigaDir()){
                QMessageBox box;
                box.setText("Did not find OpenSim.exe from taiga dir check path");
                box.exec();
                this->back();
            } else {
                m_configHandler.OUTPUT_BIN = ui->lineEditTaigaDir->text();
            }

            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            constructDatabaseConnectionStringAndDBPlugin();
            break;
        case 5:
            break;
        case 6:
            break;
        }
    } else {
        switch (id)
        {
        case 1:
            if(!checkTaigaDir()){
                QMessageBox box;
                box.setText("Did not find OpenSim.exe from taiga dir check path");
                box.exec();
                this->back();
            } else {
                m_configHandler.OUTPUT_BIN = ui->lineEditTaigaDir->text();
            }
            break;
        case 2:
            break;
        }
    }
    m_lastId = id;
}

void Wizard::onFinish_button_clicked()
{
    // read all values we have
    QMap<QString, QString> map;
    ConfigHelper::CollectAllTaggedValuesFromPage(&map, this); // get all values in wizard
    m_configHandler.WriteUIValues2ConfigBasedOnConfigSetup(map);
    // write configs to output folder
    this->m_configHandler.ApplyConfigs(m_GridConfig, m_JoinGrid, m_Regions);

    QMessageBox info;
    info.setText("Files written to bin");
    info.exec();
}

bool Wizard::checkTaigaDir()
{
    QString openSimExePath = this->ui->lineEditTaigaDir->text() + QDir::separator() + "OpenSim.exe";
    return ConfigHelper::CheckIfFileExists(openSimExePath);
}


void Wizard::on_btnAddRegion_clicked()
{
    m_RegionWizard->GoToStart();
    int retVal = this->m_RegionWizard->exec();

    if(retVal!=0){
        QMap<QString,QString> regionWizardMap;
        ConfigHelper::CollectAllTaggedValuesFromPage(&regionWizardMap, m_RegionWizard);

        QString regString = ConfigHelper::RegionMapToString(&regionWizardMap);
        regString.append("|InternalAddress=0.0.0.0|AllowAlternatePorts=False");

        QListWidget* widget = ui->listWidgetRegions;
        QMap<QString, QMap<QString,QString>* > regions = ConfigHelper::ListWidgetToMap(widget);

        QString ret = ConfigHelper::CheckRegionForConflicts(regions, &regionWizardMap);
        if(ret==""){
            QString name = regionWizardMap.value("region_name");
            name = ConfigHelper::RemoveQuotes(name);
            // update region list
            QListWidgetItem *item = new QListWidgetItem(ui->listWidgetRegions);
            item->setToolTip(regString);
            item->setText(name);
            ui->listWidgetRegions->addItem(item);
        }
        else
        {
            regionConflict = new QMessageBox(this);
            regionConflict->setText("Check:" + ret);
            regionConflict->exec();
        }
    }
}

void Wizard::on_btnUpdateRegion_clicked()
{
    QListWidgetItem * item = ui->listWidgetRegions->currentItem();
    if(item!=0)
    {
        QMap<QString,QString>* map = ConfigHelper::StringToMap(item->toolTip(), "|", "=");

        //QFormLayout* regionLayout = this->m_RegionWizard->GetRegionLayout();

        QGridLayout* regionLayout = this->m_RegionWizard->GetRegionGridLayout();

        QFormLayout* avatarLayout = this->m_RegionWizard->GetAvatarLayout();

        ConfigHelper::MapToFormLayout(map, regionLayout);
        ConfigHelper::MapToFormLayout(map, avatarLayout);

        m_RegionWizard->GoToStart();
        this->m_RegionWizard->exec();
        QMap<QString,QString> regionWizardMap;
        ConfigHelper::CollectAllTaggedValuesFromPage(&regionWizardMap, m_RegionWizard);

        // update listWidgetRegions item
        QString tooltip = ConfigHelper::RegionMapToString(&regionWizardMap);
        item->setToolTip(tooltip);
        QString name = regionWizardMap.value("region_name");
        name = ConfigHelper::RemoveQuotes(name);
        item->setText(name);
    }
}

void Wizard::on_btnBrowseDirectory_clicked()
{
    QDir cwd = QDir::current();

    QString location = ConfigHelper::SelectPathOrFolder(QString("Select taiga location"),
        this, cwd.absolutePath(), selectFolder, "");

    ui->lineEditTaigaDir->setText(location);

}

void Wizard::on_checkBoxRobust_clicked()
{
    if(this->ui->checkBoxRobust->isChecked())
    {
        ui->lineEditSimAssetUrl->setEnabled(false);
        ui->lineEditSimInventoryUrl->setEnabled(false);
    }
    else
    {
        ui->lineEditSimAssetUrl->setEnabled(true);
        ui->lineEditSimInventoryUrl->setEnabled(true);
    }
}

void Wizard::on_lineEditSimGridUrl_textChanged(QString )
{
    if(this->ui->checkBoxRobust->isChecked())
    {
        QString gridUrlText = ui->lineEditSimGridUrl->text();
        ui->lineEditSimAssetUrl->setText(gridUrlText);
        ui->lineEditSimInventoryUrl->setText(gridUrlText);
    }
}

void Wizard::constructDatabaseConnectionStringAndDBPlugin()
{    
    QString dbUser = this->ui->lineEditGridDBUser->text();
    QString dbPassword = this->ui->lineEditGridDBPassword->text();
    QString dbDataSource = this->ui->lineEditDataSource->text();
    QString dbPlugin = this->ui->comboDBPlugin->currentText();

    ui->labelGridDbPlugin->setText(dbPlugin);
    ui->labelSimDBPlugin->setText(dbPlugin);

    QString connStr = "";
    QString nhibConnStr = "";
    QString propertyProvider = "";
    QString modrexConnStr = "";
    if(dbPlugin=="OpenSim.Data.MySQL.dll")
    {
        connStr = ConnectionStringHelper::ConstructMySqlConnectionString(dbDataSource, dbUser, dbPassword);
    }
    else if (dbPlugin=="OpenSim.Data.SQLite.dll")
    {
        connStr = ConnectionStringHelper::ConstructSqliteConnectionString();
    }
    nhibConnStr = ConnectionStringHelper::ConstructNHibernateConnectionString(dbDataSource, dbUser, dbPassword, dbPlugin, propertyProvider);
    modrexConnStr = ConnectionStringHelper::ConstructModrexConnectionString(dbDataSource, dbUser, dbPassword, dbPlugin);

    ui->labelSimDBString->setText(connStr);
    ui->labelGridDBString->setText(connStr);
    ui->labelGridNhib->setText(nhibConnStr);
    ui->labelPropProv->setText(propertyProvider);
    ui->labelModRexConnStr->setText(modrexConnStr);
}

void Wizard::on_btnSaveConfigs_clicked()
{
    if(ui->lineEditSaveName->text()!="")
    {
        QDir cwd = QDir::current();
        QString name = ui->lineEditSaveName->text();

        QString saveDir = ConfigHandler::SAVE_DIR.replace("/", QDir::separator());
        QString location = ConfigHelper::SelectPathOrFolder(QString("Select configs location"),
            this, cwd.absolutePath() + QDir::separator() + saveDir, selectFolder);
        QMap<QString,QString>* uiMap = new QMap<QString,QString>();
        ConfigHelper::CollectAllTaggedValuesFromPage(uiMap, this);
        m_configHandler.SaveConfigs(m_GridConfig, m_JoinGrid, m_Regions, uiMap, name, location);
    }
    else
    {
        QMessageBox box;
        box.setText("Give name to configuration first");
        box.exec();
    }
}

void Wizard::loadDefaults()
{
    ConfigHelper::SetConfigValuesToPage(m_configHandler.m_Defaults->m_Sections, this);
    setRegionsFromIni();
}

void Wizard::setRegionsFromIni()
{
    QString key = m_configHandler.m_RegionIni->m_Sections.keys().at(0);

    foreach(QString region_name, m_configHandler.m_RegionIni->m_Sections.keys())
    {
        QMap<QString,QString>* map = m_configHandler.m_RegionIni->m_Sections.value(region_name);
        map->insert("region_name", region_name);
        QString toolTip = ConfigHelper::RegionMapToString(map);
        QListWidgetItem* item = new QListWidgetItem(ui->listWidgetRegions);

        item->setToolTip(toolTip);
        item->setText(region_name);
        ui->listWidgetRegions->addItem(item);
    }
}


void Wizard::ClearRegions()
{
    ui->listWidgetRegions->clear();
}

void Wizard::on_btnRemoveRegion_clicked()
{
    if(ui->listWidgetRegions->count()==1)
    {
        //
        m_regionNotification = new QMessageBox(this);
        m_regionNotification->setText("Must have at least one region configured, use update to update region");
        m_regionNotification->show();
        return;
    }

    int row = ui->listWidgetRegions->currentRow();
    if(row!=-1)
    {
        QListWidgetItem* item = ui->listWidgetRegions->takeItem(row);
        if(item!=0)
        {
            delete item;
        }
    }
}

void Wizard::setDatabasesFromConfig()
{
    QStringList dbNames = m_configHandler.GetDatabasesFromConfigIni();
    ui->listWidgetDBNames->clear();
    foreach(QString dbName, dbNames){
        ui->listWidgetDBNames->addItem(dbName);
    }
}

void Wizard::on_btnChangeDataBaseNames_clicked()
{
    DatabaseDialog dbd(this);
    // read config values and set them to dialog
    //QMap<QString, QString>* setDbNames = new QMap<QString, QString>();

    QMap<QString, QMap<QString,QString>* >& configIniSections = this->m_configHandler.m_ConfigSetupIni->m_Sections;
    QString simulator_db_name = getCurrentDbName(configIniSections, "sim_db_connection_string", "OpenSim.ini", "Startup:storage_connection_string");
    QString asset_service_db_name = getCurrentDbName(configIniSections, "grid_db_connection_string", "OpenSim.Server.ini", "AssetService:ConnectionString");
    QString inventory_service_db_name = getCurrentDbName(configIniSections, "grid_db_connection_string", "OpenSim.Server.ini", "InventoryService:ConnectionString");
    QString grid_service_db_name = getCurrentDbName(configIniSections, "grid_db_connection_string", "OpenSim.Server.ini", "GridService:ConnectionString");
    QString user_service_db_name = getCurrentDbName(configIniSections, "grid_db_connection_string", "UserServer_Config.xml", "Config:database_connect");
    QString messaging_service_db_name = getCurrentDbName(configIniSections, "grid_db_connection_string", "MessagingServer_Config.xml", "Config:database_connect");
    QString nhibernate_service_db_name = getCurrentDbName(configIniSections, "grid_nhibernate", "OpenSim.Server.ini", "CableBeachService:ConnectionString");
    QString modrex_db_name = getCurrentDbName(configIniSections, "modrex_db_connection_string", "addon-modules/ModreX/config/modrex.ini", "realXtend:db_connectionstring");

    QMap<QString, QString>* names = new QMap<QString, QString>();
    names->insert("simulator_db_name", simulator_db_name);
    names->insert("asset_service_db_name", asset_service_db_name);
    names->insert("inventory_service_db_name", inventory_service_db_name);
    names->insert("grid_service_db_name", grid_service_db_name);
    names->insert("user_service_db_name", user_service_db_name);
    names->insert("messaging_service_db_name", messaging_service_db_name);
    names->insert("nhibernate_service_db_name", nhibernate_service_db_name);
    names->insert("modrex_db_name", modrex_db_name);

    dbd.SetDatabaseNames(names);

    dbd.exec();
    if(dbd.result()==Accepted)
    {
        // do config editing
        QMap<QString, QString>* dbNames = dbd.GetDatabaseNames();
        QString simulator_db_name = dbNames->value("simulator_db_name");
        setCurrentDbName(configIniSections, "sim_db_connection_string", "OpenSim.ini", "Startup:storage_connection_string", dbNames->value("simulator_db_name"));
        setCurrentDbName(configIniSections, "grid_db_connection_string", "OpenSim.Server.ini", "AssetService:ConnectionString", dbNames->value("asset_service_db_name"));
        setCurrentDbName(configIniSections, "grid_db_connection_string", "OpenSim.Server.ini", "InventoryService:ConnectionString", dbNames->value("inventory_service_db_name"));
        setCurrentDbName(configIniSections, "grid_db_connection_string", "OpenSim.Server.ini", "GridService:ConnectionString", dbNames->value("grid_service_db_name"));
        setCurrentDbName(configIniSections, "grid_db_connection_string", "UserServer_Config.xml", "Config:database_connect", dbNames->value("user_service_db_name"));
        setCurrentDbName(configIniSections, "grid_db_connection_string", "MessagingServer_Config.xml", "Config:database_connect", dbNames->value("messaging_service_db_name"));
        setCurrentDbName(configIniSections, "grid_nhibernate", "OpenSim.Server.ini", "CableBeachService:ConnectionString", dbNames->value("nhibernate_service_db_name"));
        setCurrentDbName(configIniSections, "modrex_db_connection_string", "addon-modules/ModreX/config/modrex.ini", "realXtend:db_connectionstring",
                         dbNames->value("modrex_db_name"));

        m_configHandler.m_ConfigSetupIni->UpdateFile();
        QString cwd = QDir::currentPath();
        QString sep = QDir::separator();
        cwd = cwd.replace("\\", sep);
        cwd = cwd.replace("/", sep);
        //QString savePath = cwd + sep + "templates" + sep + "ConfigSetup.ini";
        QString savePath = cwd + sep + "templates";
        m_configHandler.m_ConfigSetupIni->SaveFile(savePath);
        setDatabasesFromConfig();
    }
}

void Wizard::setWritableFiles()
{
    if(m_GridConfig)
    {
        ui->listWidgetFilesToWrite->addItem("OpenSim.Server.ini");
        ui->listWidgetFilesToWrite->addItem("OpenSim.ini");
        ui->listWidgetFilesToWrite->addItem("UserServer_Config.xml");
        ui->listWidgetFilesToWrite->addItem("MessagingServer_Config.xml");
        ui->listWidgetFilesToWrite->addItem("Regions.ini");
        ui->listWidgetFilesToWrite->addItem("Modrex.ini");
    }
    else
    {
        ui->listWidgetFilesToWrite->addItem("Regions.ini");
    }
}

void Wizard::on_lineEditGridDBPassword_textChanged(QString )
{

}

QString Wizard::getCurrentDbName(QMap<QString, QMap<QString,QString>* >& configIniSections, QString sectionName, QString fileName, QString searchString)
{
    QString settingString = ((QMap<QString,QString>*)configIniSections.value(sectionName))->value(fileName);
    int p0 = settingString.indexOf(searchString,0);
    int p1 = settingString.indexOf("|", p0) + 1;
    int p2 = settingString.indexOf(")", p1);
    int len = p2 - p1;
    QString db_name = settingString.mid(p1,len);
    return db_name;
}

void Wizard::setCurrentDbName(QMap<QString, QMap<QString,QString>* >& configIniSections, QString sectionName, QString fileName, QString searchString, QString newName)
{
    // find and cut down to 3 pieces, remove piece n. 2 and attach newName, and set it to configIniSections
    QString settingString = ((QMap<QString,QString>*)configIniSections.value(sectionName))->value(fileName);
    int p0 = settingString.indexOf(searchString,0);
    int p1 = settingString.indexOf("|", p0) + 1;
    int p2 = settingString.indexOf(")", p1);
    //int len = p2 - p1;
    QString string1 = settingString.left(p1);
    //QString string2 = settingString.mid(p1, len);
    QString string3 = settingString.mid(p2);
    string1.append(newName);
    string1.append(string3);
    configIniSections.value(sectionName)->insert(fileName, string1);
}

void Wizard::on_lineEditSaveName_textChanged(QString )
{
    QString saveName = ui->lineEditSaveName->text();
    if(saveName!="")
    {
        ui->btnSaveConfigs->setEnabled(true);
    }
    else
    {
        ui->btnSaveConfigs->setEnabled(false);
    }
}

QList<int> Wizard::GetRegionPorts()
{
    QList<int> ports;
    for(int i=0;i<this->ui->listWidgetRegions->count();i++)
    {
        QListWidgetItem* qlwi = ui->listWidgetRegions->item(i);
        QString key = "sim_region_number_" + QString("%1").arg(i);
        QString val = qlwi->toolTip();
        QMap<QString,QString>* map = ConfigHelper::StringToMap(val, "|", "=");
        QString portStr = map->value("InternalPort");
        portStr = ConfigHelper::RemoveQuotes(portStr);
        int port = portStr.toInt();
        ports.append(port);
    }
    return ports;
}

void Wizard::SetDataBaseParams()
{
    QString dbPlugin = ui->comboDBPlugin->currentText();
    QString connStr = ui->labelSimDBString->text();
    QString user = "";
    QString passWord = "";
    QString dataSource = "";
    ConnectionStringHelper::DBParamsFromConnectionString(dbPlugin, connStr, user, passWord, dataSource);
    ui->lineEditDataSource->setText(dataSource);
    ui->lineEditGridDBUser->setText(user);
    ui->lineEditGridDBPassword->setText(passWord);
}


void Wizard::on_comboDBPlugin_currentIndexChanged(QString text)
{
    if(text=="OpenSim.Data.MySQL.dll")
    {
        ui->lineEditDataSource->setEnabled(true);
        ui->lineEditGridDBUser->setEnabled(true);
        ui->lineEditGridDBPassword->setEnabled(true);
    }
    else if (text=="OpenSim.Data.SQLite.dll")
    {
        ui->lineEditDataSource->setEnabled(false);
        ui->lineEditGridDBUser->setEnabled(false);
        ui->lineEditGridDBPassword->setEnabled(false);
    }
}
