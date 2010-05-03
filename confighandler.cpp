#include "confighandler.h"
#include "confighelper.h"
#include <QDir>
#include <QMessageBox>

/*
QString ConfigHandler::CONF_FOLDER = "templates";
QString ConfigHandler::OUTPUT_BIN = "bin";
//QString ConfigHandler::SAVE_DIR = "templates/SavedConfigs"; // replace "/" with QDir::separator when actually using this
QString ConfigHandler::SAVE_DIR = "savedConfs";
//*/

QString ConfigHandler::CONF_FOLDER = "templates";
QString ConfigHandler::OUTPUT_BIN = "../../bin";
QString ConfigHandler::SAVE_DIR = "savedConfs";


ConfigHandler::ConfigHandler()
{
    m_OpenSimIni = new ConfigIniFile(constructPath("OpenSim.ini"));
    m_RobustIni = new ConfigIniFile(constructPath("OpenSim.Server.ini"));
    m_UserXml = new ConfigXmlFile(constructPath("UserServer_Config.xml"));
    m_MessagingXml = new ConfigXmlFile(constructPath("MessagingServer_Config.xml"));

    //m_RegionIni;
    QString sep = QDir::separator();
    m_RegionIni = new ConfigIniFile(constructPath("Regions"+sep+"Regions.ini"));

    //m_ModRexIni
    m_ModRexIni = new ConfigIniFile(constructPath("addon-modules"+sep+"ModreX"+sep+"config"+sep+"modrex.ini"));

    m_ConfigSetupIni = new ConfigIniFile(constructPath("ConfigSetup.ini"));

    m_Defaults = new ConfigIniFile(constructPath("Defaults.ini"));
    m_Error = 0;
}

ConfigHandler::~ConfigHandler()
{
    if(m_OpenSimIni!=0){
        delete m_OpenSimIni;
        m_OpenSimIni = 0;
    }
    if(m_RobustIni!=0){
        delete m_RobustIni;
        m_RobustIni = 0;
    }
    if(m_UserXml!=0){
        delete m_UserXml;
        m_UserXml = 0;
    }
    if(m_MessagingXml!=0){
        delete m_MessagingXml;
        m_MessagingXml = 0;
    }
    if(m_RegionIni!=0){
        delete m_RegionIni;
        m_RegionIni = 0;
    }
    if(m_ModRexIni!=0){
        delete m_ModRexIni;
        m_ModRexIni = 0;
    }
}

QString ConfigHandler::constructPath(QString name)
{
    return CONF_FOLDER+QDir::separator()+name;
}

void ConfigHandler::WriteUIValues2ConfigBasedOnConfigSetup(QMap<QString, QString> uiMap)
{
    WriteValuesToConf(m_OpenSimIni, uiMap, "OpenSim.ini");
    WriteValuesToConf(m_RobustIni, uiMap, "OpenSim.Server.ini");
    WriteValuesToConf(m_ModRexIni, uiMap, "addon-modules/ModreX/config/modrex.ini");

    WriteValuesToConf(m_UserXml, uiMap, "UserServer_Config.xml");
    WriteValuesToConf(m_MessagingXml, uiMap, "MessagingServer_Config.xml");


    // Regions are special case
    WriteValuesToRegion(uiMap);
}

void ConfigHandler::WriteValuesToConf(ConfigFile* conf, QMap<QString, QString> uiMap, QString fileName)
{
    // Basically we just go through every [value] in m_ConfigSetupIni, fetch the corresponding value from uiMap and write it to ConfigIniFile* ini file
    foreach(QString section, this->m_ConfigSetupIni->m_Sections.keys())
    {
        if(!uiMap.contains(section))
        { // HACK
            if(section!="ConfigData"&&section!="Regions"){
                QMessageBox* box = new QMessageBox();
                box->setText("missing section in ui:" + section);
                box->exec();
                continue;
            }
        }

        QString valueToSet = uiMap.operator [](section);

        QMap<QString,QString>* map = m_ConfigSetupIni->m_Sections.operator [](section);
        if(map->keys().contains(fileName))
        {
            try
            {
                QString commaList = map->operator [](fileName);
                SetValueToConfiguration(commaList, conf, valueToSet);
            }
            catch(int e)
            {
                m_Error = e;
            }
        }
        if(map->keys().contains(fileName+":port"))
        {
            QString commaList = map->operator [](fileName+":port");
            // need to separate port from valueToSet (http://ip:port)
            if(valueToSet.contains(':')){
                QString port = ((QStringList)valueToSet.split(':')).operator [](2);
                SetValueToConfiguration(commaList, conf, port);
            } else {
                QMessageBox* box = new QMessageBox();
                box->setText("wrong format: " + section);
                box->exec();
            }
        }
        if(map->keys().contains(fileName+":ip"))
        {
            QString commaList = map->operator [](fileName+":ip");
            // need to separate ip from valueToSet
            if(valueToSet.contains(':')){
                QString slash_ip = ((QStringList)valueToSet.split(':')).operator [](1);
                QString ip = slash_ip.split('/', QString::SkipEmptyParts).operator [](0);
                SetValueToConfiguration(commaList, conf, ip);
            } else {
                QMessageBox* box = new QMessageBox();
                box->setText("wrong format: " + section);
                box->exec();
            }
        }
    }
}

void ConfigHandler::WriteValuesToRegion(QMap<QString, QString> uiMap)
{
    QMap<QString, QString> regions;
    foreach(QString key, uiMap.keys()){
        if(key.startsWith("sim_region_number_")){
            regions.insert(key, uiMap.operator [](key));
        }
    }
    m_RegionIni->m_Sections.clear();
    m_RegionIni->m_SectionOrder.clear();
    //m_RegionIni->m_SectionKeyComments.clear();
    m_RegionIni->m_SectionKeyOrders.clear();


    foreach(QString key, regions.keys())
    {
        QString name;
        QString data = regions.operator [](key);
        QStringList pairs = data.split("|");
        QMap<QString, QString>* regionValues = new QMap<QString, QString>();
        QList<QString>* sectionKeyOrder = new QList<QString>();

        foreach(QString pair, pairs)
        {
            QStringList split = pair.split("=");
            regionValues->insert(split.operator [](0),split.operator [](1));
            sectionKeyOrder->append(split.operator [](0));
            if(split.operator [](0)=="region_name")
            {
                name = split.operator [](1);
            }
        }
        m_RegionIni->m_Sections.insert(name, regionValues);
        m_RegionIni->m_SectionOrder.append(name);
        m_RegionIni->m_SectionKeyOrders.insert(name, sectionKeyOrder);
        m_RegionIni->m_SectionKeyComments.insert(name, new QMap<QString,QString>());
        //m_RegionIni->m_SectionKeyOrders()
    }
    m_RegionIni->UpdateFile();
    //QString sep = QDir::separator();
    //m_RegionIni->SaveFile(OUTPUT_BIN+sep+"Regions");
}


void ConfigHandler::SetValueToConfiguration(QString commaListOfDestinies, ConfigFile* conf, QString valueToSet)
{
    if(conf->ConfigType=="ConfigIniFile")
    {
        SetValueToConf(commaListOfDestinies, (ConfigIniFile*) conf, valueToSet);
    }
    else if(conf->ConfigType=="ConfigXmlFile")
    {
        SetValueToConf(commaListOfDestinies, (ConfigXmlFile*) conf, valueToSet);
    }
    /*
    QString className = conf->metaObject()->className();
    if(className =="ConfigIniFile")
    {
        SetValueToConf(commaListOfDestinies, (ConfigIniFile*) conf, valueToSet);
    }
    else if(className =="ConfigXmlFile")
    {
        SetValueToConf(commaListOfDestinies, (ConfigXmlFile*) conf, valueToSet);
    }
    //*/
}

void ConfigHandler::SetValueToConf(QString commaListOfDestinies, ConfigIniFile* ini, QString valueToSet)
{
    QStringList list = commaListOfDestinies.split(','); // list of ':' separated section and ini set item: like Network:user_server_url
    foreach(QString strPair, list)
    {
        if(strPair.contains(':')){
            QStringList pair = strPair.split(':');
            QString sect = pair.operator [](0);
            QString iniKey = pair.operator [](1);
            bool ret = CheckSetValueTranslation(iniKey, valueToSet);
            if(ret)
            {
                ini->SetConfigItem(sect, iniKey, valueToSet);
            }
            else
            {
                ini->RemoveConfigItem(sect, iniKey, valueToSet);
            }
        }
        else
        {
            throw "Malformed configuration app conf file";
        }
    }
}

void ConfigHandler::SetValueToConf(QString commaListOfDestinies, ConfigXmlFile* xml, QString valueToSet)
{
    QStringList list = commaListOfDestinies.split(','); // list of ':' separated section and ini set item: like Network:user_server_url
    QMap<QString,QString> attributes;
    foreach(QString attr, list)
    {
        if(CheckSetValueTranslation(attr, valueToSet)){
            attributes.insert(attr, valueToSet);
        }
    }
    xml->SetConfigElement("Config", attributes);
}

bool ConfigHandler::CheckSetValueTranslation(QString& destiny, QString& valueToSet)
{
    //QRegExp qreg("(.*|.*)");
    //if(destiny.contains(qreg))
    if(destiny.contains("("))
    {
        if(destiny.contains("(+"))
        {
            SetValueAppend(destiny, valueToSet);
            return true;
        }
        else if(destiny.contains("(#!"))
        {
            return SetValueCheckRemove(destiny, valueToSet, true);
        }
        else if(destiny.contains("(#"))
        {
            return SetValueCheckRemove(destiny, valueToSet, false);
        }
        else
        {
            SetValueChange(destiny, valueToSet);
            return true;
        }
    }
    return true;
}

void ConfigHandler::SetValueChange(QString& destiny, QString& valueToSet)
{
    //int start = destiny.indexOf(qreg);
    int start = destiny.indexOf("(");
    int end = destiny.indexOf(")", start);
    QString pair = destiny.mid(start, end);
    QString oldString = pair.split('|').at(0);
    oldString.remove("(");
    QString newString = pair.split('|').at(1);
    newString.remove(")");
    valueToSet = valueToSet.replace(oldString, newString);
    destiny.truncate(start);
}

void ConfigHandler::SetValueAppend(QString& destiny, QString& valueToSet)
{
    valueToSet = ConfigHelper::RemoveQuotes(valueToSet);
    int start = destiny.indexOf("(+") + 2;
    int end = destiny.indexOf(")");
    int length = end-start;

    QString appendString = destiny.mid(start, length);
    foreach(QString pair, appendString.split(":"))
    {
        QStringList split = pair.split("|");
        QString compare = split.at(0);

        if(valueToSet==compare)
        {
            valueToSet.append(":"+split[1]);
            break;
        }
    }
    valueToSet = ConfigHelper::SetQuotes(valueToSet);
    destiny.truncate(start-2);
}

bool ConfigHandler::SetValueCheckRemove(QString& destiny, QString& valueToSet, bool inverse)
{
    valueToSet = ConfigHelper::RemoveQuotes(valueToSet);

    int start;
    if(inverse){ start = destiny.indexOf("(#") + 3; }
    else {start = destiny.indexOf("(#") + 2;}
    int end = destiny.indexOf(")");
    int length = end-start;

    QString checkString = destiny.mid(start, length);
    if(inverse){destiny.truncate(start-3);}
    else{destiny.truncate(start-2);}
    if(checkString==valueToSet)
    {
        valueToSet = ConfigHelper::SetQuotes(valueToSet);
        if(inverse)
        {
            //destiny = ";" + destiny;
            return true;
        }
        else
            return false;
    }
    else
    {
        valueToSet = ConfigHelper::SetQuotes(valueToSet);
        if(inverse)
            return false;
        else
            //destiny = ";" + destiny;
            return true;
    }
}

void ConfigHandler::ApplyConfigs(bool grid, bool sim, bool regions)
{
    if(grid) // write OpenSim.Server.ini, UserServer_Config.xml, MessagingServer_Config.xml
    {
        ApplyGridConfig();
    }
    if(sim) // write OpenSim.ini
    {
        ApplySimConfig();
    }
    if(regions) // write Regions.ini
    {
        ApplyRegionConfig();
    }
}

void ConfigHandler::SaveConfigs(bool grid, bool sim, bool regions, QMap<QString,QString>* uiMap, QString name, QString path)
{
    SetUIMapValuesToConfConfiguration(uiMap);

    QMap<QString, QString>* confData = new QMap<QString, QString>();
    QList<QString>* keyOrder = new QList<QString>();

    if(grid){confData->insert("grid", "true"); keyOrder->insert(0, "grid");}
    if(sim){confData->insert("sim", "true"); keyOrder->insert(0, "sim");}
    if(regions){confData->insert("regions", "true"); keyOrder->insert(0, "regions");}
    m_ConfigSetupIni->m_Sections.insert("ConfigData", confData);

    m_ConfigSetupIni->m_SectionKeyOrders.insert("ConfigData", keyOrder);
    m_ConfigSetupIni->m_SectionOrder.insert(0, "ConfigData");
    m_ConfigSetupIni->m_SectionKeyComments.insert("ConfigData", new QMap<QString,QString>());
    m_ConfigSetupIni->UpdateFile();

    QString keepName = m_ConfigSetupIni->ConfigFileName;
    m_ConfigSetupIni->ConfigFileName = name;
    QString sep = QDir::separator();
    m_ConfigSetupIni->SaveFileWithAbsolutePath(path + sep + name);
    m_ConfigSetupIni->ConfigFileName = keepName;
}

void ConfigHandler::ApplyGridConfig()
{
    m_RobustIni->UpdateFile();
    m_UserXml->UpdateFile();
    m_MessagingXml->UpdateFile();;
    m_ModRexIni->UpdateFile();

    //write to OUTPUT_BIN
    m_RobustIni->SaveFile(OUTPUT_BIN);
    m_UserXml->SaveFile(OUTPUT_BIN);
    m_MessagingXml->SaveFile(OUTPUT_BIN);
    QString sep = QDir::separator();

}

void ConfigHandler::ApplySimConfig()
{
    m_OpenSimIni->UpdateFile();
    m_OpenSimIni->SaveFile(OUTPUT_BIN);

    QString sep = QDir::separator();

    m_RegionIni->UpdateFile();
    m_RegionIni->SaveFile(OUTPUT_BIN+sep+"Regions");
    m_ModRexIni->UpdateFile();
    m_ModRexIni->SaveFile(OUTPUT_BIN+sep+"addon-modules"+sep+"ModreX"+sep+"config");
}

void ConfigHandler::ApplyRegionConfig()
{
    m_RegionIni->UpdateFile();
    m_RegionIni->SaveFile(OUTPUT_BIN+ QDir::separator() + "Regions"); // TODO: check this
}


void ConfigHandler::SetUIMapValuesToConfConfiguration(QMap<QString,QString>* uiMap)
{
    // go thru each key in ui map
    // find it in conf map section
    // add new key "value" to section in conf with ui map value
    // collect regions and add them too
    QMap<QString,QString>* regions = new QMap<QString,QString>();
    QList<QString>* regionKeys = new QList<QString>();
    foreach(QString uiKey, uiMap->keys())
    {
        foreach(QString confKey, m_ConfigSetupIni->m_Sections.keys())
        {
            if(confKey==uiKey)
            {
                QMap<QString,QString>* section = m_ConfigSetupIni->m_Sections.operator [](confKey);
                section->insert("Taiga-Config-Value", uiMap->operator [](uiKey));
            }
        }
        if(uiKey.startsWith("sim_region_number_"))
        {
            regions->insert(uiKey, "\"" + uiMap->operator [](uiKey) + "\"");
            regionKeys->insert(0, uiKey);
        }
    }
    m_ConfigSetupIni->m_Sections.insert("Regions",regions);
    m_ConfigSetupIni->m_SectionOrder.insert(0, "Regions");
    m_ConfigSetupIni->m_SectionKeyOrders["Regions"] = regionKeys;
    m_ConfigSetupIni->m_SectionKeyComments["Regions"] = new QMap<QString, QString>();
}

QStringList ConfigHandler::GetDatabasesFromConfigIni()
{
    // search each ${Database}$ string from config ini
    QStringList dbNames;

    QTextStream stream(&m_ConfigSetupIni->ConfigContents);
    QString line = "";

    while(line!=QString::null)
    {
        line = stream.readLine();
        line = line.trimmed();
        if(!line.startsWith(';'))
        {
            int index = 0;
            while(index!=-1){
                index = line.indexOf("${Database}$", index);
                if(index!=-1){
                    // parse database name
                    int start = line.indexOf("|", index)+1;
                    int end = line.indexOf(")", start);
                    index = end;
                    int len = end-start;
                    QString dbName = line.mid(start, len);
                    dbNames.append(dbName);
                }
            }
        }
    }
    return dbNames;
}
