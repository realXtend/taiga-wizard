#include "configinifile.h"
#include <QStringList>
#include <QSettings>
#include <QDir>

// constructor
ConfigIniFile::ConfigIniFile(QString fileName):ConfigFile(fileName)
{
    ConfigType = "ConfigIniFile";
    if(this->loadSuccess) // file read succeeded
    {
        bool sectionFound=false;
        bool errors = false;
        ConfigContents .remove('\r');
        QStringList lines = ConfigContents.split('\n');
        QString section = "";
        QString currentComment;
        foreach(QString line, lines)
        {
            line = line.trimmed();

            if(line.startsWith('[')&&line.endsWith(']')) // it's section
            {
                sectionFound=true;
                section = line.mid(1, line.length()-2);
                if(currentComment!=QString::null && currentComment!="")
                {
                    m_SectionComments.insert(section, currentComment);
                    currentComment.clear();
                }
                m_SectionOrder.append(section);
                m_SectionKeyComments.insert(section, new QMap<QString, QString>());
                m_SectionKeyOrders.insert(section, new QList<QString>());
                this->m_Sections.insert(section, new QMap<QString, QString>());
            }
            else
            {
                QString key;
                QString value;
                if(!line.startsWith(';')&&line!=""){
                    if(ParseConfigItem(line, key, value)){
                        if(!sectionFound)
                        {
                            if(currentComment!=""){
                                m_GlobalKeyComments.insert(key, currentComment);
                                currentComment.clear();
                            }
                            m_GlobalsOrder.append(key);
                            this->m_GlobalItems.insert(key, value);
                        }
                        if(sectionFound)
                        {
                            if(currentComment!=QString::null && currentComment!="")
                            {
                                ((QMap<QString, QString>*)m_SectionKeyComments.value(section))->insert(key, currentComment);
                                currentComment.clear();
                            }
                            ((QList<QString>*)m_SectionKeyOrders.value(section))->append(key);
                            m_Sections.value(section)->insert(key, value);
                        }
                    } else {
                        errors = true;
                    }
                } else {
                    currentComment.append(line + "\n");
                }
            }
        }
    }
}

ConfigIniFile::~ConfigIniFile()
{
    foreach(QString key, m_Sections.keys()){
        delete m_Sections[key];
    }
    foreach(QString key, m_SectionKeyOrders.keys()){
        delete m_SectionKeyOrders[key];
    }
    foreach(QString key, m_SectionKeyComments.keys()){
        delete m_SectionKeyComments[key];
    }
}

bool ConfigIniFile::ParseConfigItem(QString str, QString& key, QString &value)
{
     QStringList split = str.split('=');
     if(split.length()==2){
         key = split.at(0).trimmed();
         value = split.at(1).trimmed();
         return true;
     }
     else if(split.length()>2)
     {
         QString firstarg = split.at(1);
         firstarg = firstarg.trimmed();
         QString lastarg = split.at(split.length()-1);
         lastarg = lastarg.trimmed();
         if(firstarg.startsWith("\"")&&lastarg.endsWith("\"")) // checking that rest of the '=' s are inside ""'s
         {
            key = split.at(0).trimmed();
            split.removeAt(0);
            value = split.join("=");
            /*
            for(int i=1;i<split.length();i++)
            {
                value.append(split.at(i));
            } //*/
         }
         return true;
     }
     else
     {
         key = "";
         value = "";
         return false;
     }
}

void ConfigIniFile::SetConfigItem(QString section, QString key, QString value)
{
    if(section!=0&&key!=0)
    {
        if(section!=QString::null&&section!="")
        {
            m_Sections.value(section)->insert(key, value);
        }
        else
        {
            m_GlobalItems.insert(key,value);
        }
    }
    UpdateFile();
}

void ConfigIniFile::RemoveConfigItem(QString section, QString key, QString value)
{
    if(section!=QString::null && section!=""){
        m_Sections.value(section)->remove(key);
    } else {m_GlobalItems.remove(key);}
    UpdateFile();
}

void ConfigIniFile::UpdateFile() // will lose comments on ini file
{
    this->ConfigContents.clear();
    /*
    //Write globals
    foreach(QString key, m_GlobalItems.keys()){
        ConfigContents.append(key+"="+m_GlobalItems[key]+"\n\r");
    }
    //Write sections
    ConfigContents.append(";Config creator file\n\r");
    foreach(QString key, m_Sections.keys()){
        ConfigContents.append("["+key+"]\n\r");
        IniSection* section = m_Sections[key];
        foreach(QString itemKey, section->keys())
        {
            QString value = section->value(itemKey);
            itemKey = itemKey.trimmed();
            if(itemKey!=""){
                ConfigContents.append(itemKey+"="+value+"\n\r");
            }
        }
    }
    //*/

    foreach(QString key, m_GlobalsOrder)
    {
        if(m_GlobalKeyComments.contains(key))
            ConfigContents.append(m_GlobalKeyComments.value(key));
        ConfigContents.append(key+"="+m_GlobalItems.value(key)+"\n\r");
    }
    foreach(QString key, m_SectionOrder)
    {
        if(m_SectionComments.contains(key))
            ConfigContents.append(m_SectionComments.value(key));
        ConfigContents.append("["+key+"]\n\r");

        QList<QString>* sectionKeysOrder = m_SectionKeyOrders.value(key);
        QMap<QString,QString>* sectionKeyValuePairs = m_Sections.value(key);
        QMap<QString,QString>* sectionKeyComments = m_SectionKeyComments.value(key);

        if(sectionKeyValuePairs->contains("Taiga-Config-Value")) // saving template
        {
            QString val = sectionKeyValuePairs->value("Taiga-Config-Value");
            QString line = "Taiga-Config-Value=";
            line.append(val);
            ConfigContents.append(line + "\n\r");
        }

        foreach(QString itemKey, *sectionKeysOrder)
        {
            QString value = sectionKeyValuePairs->value(itemKey);
            itemKey = itemKey.trimmed();
            if(itemKey!=QString::null && itemKey!=""){
                if(sectionKeyComments->contains(itemKey))
                    ConfigContents.append(sectionKeyComments->value(itemKey));
                ConfigContents.append(itemKey+"="+value+"\n\r");
            }
        }
    }
}
