#ifndef CONFIGHANDLER_H
#define CONFIGHANDLER_H

#include "configinifile.h"
#include "configxmlfile.h"
#include <QMap>
#include <QFormLayout>
#include <QTextEdit>
#include <QComboBox>

class ConfigSetup
{
    bool simSetup;
    bool gridSetup;
    ConfigIniFile m_OpenSimIni;
    ConfigIniFile m_RobustIni;
    ConfigXmlFile m_UserXml;
    ConfigXmlFile m_MessagingXml;
    ConfigIniFile m_RegionIni;
};


class ConfigHandler
{
public:
    ConfigHandler();
    ~ConfigHandler();

    // config templates, loaded at startup
    ConfigIniFile* m_OpenSimIni;
    ConfigIniFile* m_RobustIni;
    ConfigXmlFile* m_UserXml;
    ConfigXmlFile* m_MessagingXml;
    ConfigIniFile* m_RegionIni;

    ConfigIniFile* m_ModRexIni;

    ConfigIniFile* m_Defaults;

    // config setup for modifying templates from ui inputs
    ConfigIniFile* m_ConfigSetupIni;

    QMap<QString,QString> *m_GridConfMap;
    QMap<QString,QString> *m_SimNetworkConfMap;
    QMap<QString,QMap<QString,QString>* > *m_SimRegionConfMap;

    void WriteUIValues2ConfigBasedOnConfigSetup(QMap<QString, QString> uiMap);
    void ApplyConfigs(bool grid, bool sim, bool regions);
    void SaveConfigs(bool grid, bool sim, bool regions, QMap<QString,QString>* uiMap, QString name, QString path="");

    QStringList GetDatabasesFromConfigIni();

    static QString OUTPUT_BIN;
    static QString SAVE_DIR;
    static QString CONF_FOLDER;

    int m_Error;

private:
    QString constructPath(QString name);
    void WriteValuesToConf(ConfigFile* conf, QMap<QString, QString> uiMap, QString fileName);
    void WriteValuesToRegion(QMap<QString, QString> uiMap);
    void SetValueToConfiguration(QString commaListOfDestinies, ConfigFile* ini, QString valueToSet);
    void SetValueToConf(QString commaListOfDestinies, ConfigIniFile* ini, QString valueToSet);
    void SetValueToConf(QString commaListOfDestinies, ConfigXmlFile* xml, QString valueToSet);

    // VALUE TRANSLATIONS
    bool CheckSetValueTranslation(QString& destiny, QString& valueToSet);
    void SetValueChange(QString& destiny, QString& valueToSet);
    void SetValueAppend(QString& destiny, QString& valueToSet);
    bool SetValueCheckRemove(QString& destiny, QString& valueToSet, bool inverse);

    void ApplyGridConfig();
    void ApplySimConfig();
    void ApplyRegionConfig();


    void SetUIMapValuesToConfConfiguration(QMap<QString,QString>* uiMap);
};


#endif // CONFIGHANDLER_H
