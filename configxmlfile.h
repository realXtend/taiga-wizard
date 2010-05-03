#ifndef CONFIGXMLFILE_H
#define CONFIGXMLFILE_H

#include <QString>
#include <QMap>
#include <QtXml>
#include <QDomDocument>
#include "configfile.h"

class ConfigXmlFile : public ConfigFile
{
public:
    //ConfigXmlFile();
    ConfigXmlFile(QString fileName);
    ~ConfigXmlFile();

    // These 2 functions work only for the case of MessagingServer_Config.xml and UserServer_Config.xml
    // for setting Config elements attributes, in case of change in configs these need to be rewritten
    // these just get the elements under parent <Root> tag
    QString SetConfigElement(QString name, QMap<QString,QString> attributes, QString value="");
    QString GetConfigElement(QString name, QMap<QString,QString> & attributes, QString & value);
    void UpdateFile();

private:
    QString GetDomElement(QString name, QDomElement& element);


public:
    QDomDocument m_DomDocument;
    QDomElement m_DocElem;
};

#endif // CONFIGXMLFILE_H
