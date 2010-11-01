#ifndef CONFIGINIFILE_H
#define CONFIGINIFILE_H

#include <QString>
#include <QMap>
#include <QList>
#include "configfile.h"

#define IniSection QMap<QString, QString>


// Reads loads, edits and writes ini file
// Note!!: Does not read included ini files inside ini file
class ConfigIniFile : public ConfigFile
{
public:

    ConfigIniFile(QString fileName);
    ~ConfigIniFile();
    //void Save(QString name);


    void SetConfigItem(QString section, QString key, QString value);
    void UpdateFile();
    void RemoveConfigItem(QString section, QString key, QString value);

private:
    bool ParseConfigItem(QString str, QString& key, QString &value);

public:
    QMap<QString,QString> m_GlobalItems;
    QMap<QString,QMap<QString,QString>* > m_Sections;

private:
    // Unimplemented stuff
    // preserve order
    QList<QString> m_GlobalsOrder;
public:
    QList<QString> m_SectionOrder;
    QMap<QString,QList<QString>* > m_SectionKeyOrders;
private:
    // preserve comments

    QMap<QString, QString> m_GlobalKeyComments;
    QMap<QString, QString> m_SectionComments;
public:
    QMap<QString, QMap<QString, QString>* > m_SectionKeyComments;

};

#endif // CONFIGINIFILE_H
