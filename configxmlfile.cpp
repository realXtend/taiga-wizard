#include "configxmlfile.h"

#include "confighelper.h"


ConfigXmlFile::ConfigXmlFile(QString fileName) : ConfigFile(fileName), m_DomDocument(fileName)
{
    ConfigType = "ConfigXmlFile";

    //QString *error;
    //doc.setContent(this->ConfigContents, error);
    if(!m_DomDocument.setContent(this->ConfigContents))
    {
        this->loadSuccess = false;
        return;
    }
    this->loadSuccess = true;
    m_DocElem = m_DomDocument.documentElement();
}

ConfigXmlFile::~ConfigXmlFile()
{
}


QString ConfigXmlFile::SetConfigElement(QString name, QMap<QString,QString> attributes, QString value)
{
    QDomElement e;
    QString ret = GetDomElement(name, e);
    if(ret!="Success"){return ret;}
    if(name==e.nodeName())
    {
        QDomNamedNodeMap attrMap = e.attributes();
        foreach(QString key, attributes.keys())
        {
            QString attrKey = key;
            if(attrKey.startsWith("Config:")){
                attrKey = attrKey.mid(7);
            }
            if(attrMap.contains(attrKey))
            {
                QDomNode node = attrMap.namedItem(attrKey);
                QDomAttr attr = node.toAttr();
                //attr.setNodeValue(attributes[key]);
                QString val = attributes.value(key);
                val = ConfigHelper::RemoveQuotes(val);
                attr.setValue(val);
                //attr.setValue(attributes[key]);
            }
            else
            {
                return key + " attribute not found";
                if(value!=""){
                    //TODO: set value
                }
                /*
                QDomAttr* attr = new QDomAttr();
                attr->setValue(attributes[key]);
                //*/
            }
        }
        return "Success";
    }
    return "Internal error";
}

QString ConfigXmlFile::GetConfigElement(QString name, QMap<QString,QString> & attributes, QString & value)
{
    /*
    QDomNodeList nodelist = m_DocElem.elementsByTagName(name);
    if(nodelist.count()>1)
    {
        return "Ambiguous";
    }
    if(nodelist.count()==0)
    {
        return "Not found";
    }
    QDomElement e = ((QDomNode)nodelist.at(0)).toElement();
    //*/

    QDomElement e;
    QString ret = GetDomElement(name, e);
    if(ret!="Success"){return ret;}

    if(name==e.nodeName()){
        QDomNamedNodeMap attrMap = e.attributes();
        for(int j=0;j<attrMap.count();j++)
        {
            QDomNode n = attrMap.item(j);
            QDomAttr attr = n.toAttr();
            //QDomAttr attr = ((QDomAttr)attrMap.item(j)).toAttr();
            attributes.insert(attr.name(), attr.value());
        }
        value = e.nodeValue();
        return "Success";
    }
    return "Internal error";
}

QString ConfigXmlFile::GetDomElement(QString name, QDomElement& element)
{
    QDomNodeList nodelist = m_DocElem.elementsByTagName(name);
    if(nodelist.count()>1)
    {
        return "Ambiguous";
    }
    if(nodelist.count()==0)
    {
        return "Not found";
    }
    element = ((QDomNode)nodelist.at(0)).toElement();
    return "Success";
}

void ConfigXmlFile::UpdateFile()
{
    this->ConfigContents.clear();
    this->ConfigContents = m_DomDocument.toString();
}
