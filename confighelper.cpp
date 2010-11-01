#include "confighelper.h"
#include <QFileDialog>
#include <QLayout>
#include <QLayoutIterator>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QListWidget>
#include <QLabel>

ConfigHelper::ConfigHelper()
{
}

QString ConfigHelper::SelectPathOrFolder(QString caption, QWidget* parent, QString dir, FileDialogType type, QString filter)
{
    switch (type){
    case saveFile:
        return QFileDialog::getOpenFileName(parent, caption, dir, filter);
        break;
    case openFile:
        return QFileDialog::getOpenFileName(parent, caption, dir, filter);
        break;
    case selectFolder:
        return QFileDialog::getExistingDirectory(parent, caption, dir);
        break;
    }
    return "";
    //QString location = QFileDialog::getOpenFileName(parent, caption, dir);
    //QString location = QFileDialog::getExistingDirectory(parent, caption, dir);
    //return location;
}

QString ConfigHelper::CheckURLsFromMap(QMap<QString,QString> map)
{
    foreach(QString key, map.keys())
    {
        if(key!="sim_http_port")
        {
            QString url = map.value(key);
            QUrl check(url, QUrl::StrictMode);
            QString scheme = check.scheme();
            if(scheme!="http")
                return key;
            if(!check.isValid())
                return key;
        }
        if(key=="sim_http_port")
        {
            QString port = map.value(key);
            bool ok;
            port.toUInt(&ok);
            if(!ok)
                return key;
        }
    }
    return "";
}

QMap<QString,QString>* ConfigHelper::FormLayoutToMap(QFormLayout* formLayout)
{
    QMap<QString,QString>* map = new QMap<QString,QString>();

    for(int i=0;i<formLayout->count();i++)
    {
        QWidget* widget = formLayout->itemAt(i)->widget();
        QString className = widget->metaObject()->className();

        if(className=="QLineEdit")
        {
            QString key = ((QLineEdit*)widget)->accessibleName();
            QString value = ((QLineEdit*)widget)->text();
            map->insert(key, value);
        }
    }
    return map;
}

void ConfigHelper::MapToFormLayout(QMap<QString,QString>* map, QFormLayout* formLayout)
{
    foreach(QString key, map->keys())
    {
        for(int i=0;i<formLayout->count();i++)
        {
            QWidget* widget = formLayout->itemAt(i)->widget();
            QString className = widget->metaObject()->className();

            if(className=="QLineEdit")
            {
                QString name = ((QLineEdit*)widget)->accessibleName();
                if(key==name)
                {
                    QString value = map->value(key);
                    value = RemoveQuotes(value);
                    ((QLineEdit*)widget)->setText(value);
                }
            }
        }
    }
}

void ConfigHelper::MapToFormLayout(QMap<QString,QString>* map, QGridLayout* formLayout)
{
    foreach(QString key, map->keys())
    {
        for(int i=0;i<formLayout->count();i++)
        {
            QWidget* widget = formLayout->itemAt(i)->widget();
            QString className = widget->metaObject()->className();

            if(className=="QLineEdit")
            {
                QString name = ((QLineEdit*)widget)->accessibleName();
                if(key==name)
                {
                    QString value = map->value(key);
                    value = RemoveQuotes(value);
                    ((QLineEdit*)widget)->setText(value);
                }
            }
        }
    }
}


void ConfigHelper::CollectAllTaggedValuesFromPage(QMap<QString,QString>* map, QWidget* widget)
{
    //if (obj->isWidgetType()) {
    //QWidget* widget = (QWidget*) obj;
    QObjectList children = widget->children();
    foreach(QObject* obj, children)
    {
        if(obj->isWidgetType())
        {
            QString name = obj->metaObject()->className();
            if(name=="QFormLayout"){
                QMap<QString,QString>* layoutMap = FormLayoutToMap((QFormLayout*)obj);
                CopyMapToMap(map, layoutMap);
            }
            else if(name=="QComboBox"){
                QComboBox* combo = (QComboBox*)obj;
                QString key = combo->accessibleName();
                if(key!=""){
                    QString value = combo->currentText();
                    value = SetQuotes(value);
                    map->insert(key, value);
                }
            }
            else if(name=="QTextEdit"){
                QTextEdit* textEdit = (QTextEdit*)obj;
                QString key = textEdit->accessibleName();
                if(key!=""){
                    QString str = textEdit->toPlainText();
                    // remove any linefeeds
                    //str = str.replace('\n', ' ');
                    str = str.remove('\n');
                    str = str.remove('\r');
                    str = SetQuotes(str);
                    map->insert(key, str);
                }
            }
            else if(name=="QLineEdit"){
                QLineEdit* lineEdit = (QLineEdit*)obj;
                QString key = lineEdit->accessibleName();
                if(key!=""){
                    QString str = lineEdit->text();
                    str = SetQuotes(str);
                    map->insert(key, str);
                }
            }
            else if(name=="QLabel")
            {
                QLabel* label = (QLabel*)obj;
                QString key = label->accessibleName();
                if(key!=""){
                    QString str = label->text();
                    str = SetQuotes(str);
                    map->insert(key, str);
                }
            }
            else if(name=="QListWidget"){
                QListWidget* listWidget = (QListWidget*)obj;
                if(listWidget->accessibleName()=="sim_regions")// This is hardcoded for regions, making this generic would be too complicated
                {
                    for(int i=0;i<listWidget->count();i++)
                    {
                        QListWidgetItem* qlwi = listWidget->item(i);
                        QString key = "sim_region_number_" + QString("%1").arg(i);
                        QString val = qlwi->toolTip();
                        //val = RemoveQuotes(val);
                        //SetQuotes()
                        map->insert(key, val);
                    }
                }
            }
            else{
                //QWidget* w = qobject_cast<QWidget*>(obj);
                CollectAllTaggedValuesFromPage(map, (QWidget*)obj);
            }
        }
        else {
            CollectAllTaggedValuesFromPage(map, (QWidget*)obj);
        }
    }
}

void ConfigHelper::CopyMapToMap(QMap<QString,QString>* destiny, QMap<QString,QString>* source)
{
    foreach(QString key, source->keys())
    {
        destiny->insert(key, source->value(key));
    }
}

QString ConfigHelper::CheckRegionForConflicts(QMap<QString, QMap<QString,QString>* > regions, QMap<QString,QString>* newRegion)
{
    // region_name, RegionUUID, Location, InternalPort
    QString region_name = newRegion->value("region_name");
    QString RegionUUID = newRegion->value("RegionUUID");
    QString Location = newRegion->value("Location");
    QString InternalPort = newRegion->value("InternalPort");

    foreach(QString name, regions.keys())
    {
        if(region_name==name)
        {
            return "Name";
        }
        QMap<QString,QString>* reg = regions.value(name);
        if(RegionUUID==reg->value("RegionUUID"))
        {
            return "RegionUUID";
        }
        if(Location==reg->value("Location"))
        {
            return "Location";
        }
        if(InternalPort==reg->value("InternalPort"))
        {
            return "InternalPort";
        }
    }
    return "";
}

QString ConfigHelper::CheckRegionForConflicts(QListWidget* regions, QMap<QString,QString>* newRegion)
{
    QMap<QString, QMap<QString,QString>* > regionsMap;
    /*
    for(int i=0; i<regions.count();i++)
    {
        QMap<QString,QString>* region = new QMap<QString,QString>();
        QListWidgetItem item = regions.item(i);
        QString name = item.text();
        QString toolTip = item.toolTip();
        QStringList split = toolTip.split("|");
        foreach(QString pair, split)
        {
            region->insert(pair.split("=").at(0), pair.split("=").at(1));
        }
        region->insert("region_name", name);
        regionsMap.insert(region);
    }//*/
    regionsMap = ListWidgetToMap(regions);
    return CheckRegionForConflicts(regionsMap, newRegion);
}

QString ConfigHelper::RegionMapToString(QMap<QString,QString>* map)
{
    QString retVal = "";
    foreach(QString key, map->keys())
    {
        QString value = map->value(key);
        QString keyValue = key.remove("|"); // will use "|" as separator so its forbidden
        QString valValue = value.remove("|");
        retVal.append(keyValue+"="+valValue+"|");
    }
    if(retVal.endsWith("|"))
        retVal.chop(1);
    return retVal;
}

QMap<QString, QMap<QString,QString>* > ConfigHelper::ListWidgetToMap(QListWidget* listWidget)
{
    QMap<QString, QMap<QString,QString>* > regionsMap;
    for(int i=0; i<listWidget->count();i++)
    {
        QMap<QString,QString>* region = new QMap<QString,QString>();
        QListWidgetItem* item = listWidget->item(i);
        QString name = item->text();
        QString toolTip = item->toolTip();
        QStringList split = toolTip.split("|");
        foreach(QString pair, split)
        {
            region->insert(pair.split("=").at(0), pair.split("=").at(1));
        }
        region->insert("region_name", name);
        regionsMap.insert(name, region);
    }
    return regionsMap;
}

QMap<QString,QString>* ConfigHelper::StringToMap(QString str, QString separator, QString pairSeparator)
{
    QMap<QString,QString>* map = new QMap<QString,QString>();
    QStringList pairs = str.split(separator);
    foreach(QString pair, pairs)
    {
        if(pair.split(pairSeparator).count()==2){
            map->insert(pair.split(pairSeparator).at(0), pair.split(pairSeparator).at(1));
        }
    }
    return map;
}

void ConfigHelper::SetConfigValuesToPage(QMap<QString,QMap<QString,QString>* > sections, QWidget* widget)
{
    QMap<QString,QString>* valueMap = ConfigSectionsToValueMap(sections);
    SetValuesToUI(valueMap, widget);
}

QMap<QString,QString>* ConfigHelper::ConfigSectionsToValueMap(QMap<QString,QMap<QString,QString>* > sections)
{
    QMap<QString,QString>* map = new QMap<QString,QString>();
    foreach(QString key, sections.keys())
    {
        if(key!="Regions")
        {
            QMap<QString,QString>* section = sections.value(key);
            QString value = section->value("Taiga-Config-Value");
            value = ConfigHelper::RemoveQuotes(value);
            map->insert(key, value);
        }
        else
        {
            QMap<QString,QString>* regions = sections.value(key);
            foreach(QString region, regions->keys())
            {
                region = ConfigHelper::RemoveQuotes(region);
                map->insert(region, regions->value(region));
            }
        }
    }
    return map;
}

void ConfigHelper::SetValuesToUI(QMap<QString,QString>* map, QWidget* widget)
{
    QObjectList children = widget->children();
    foreach(QObject* obj, children)
    {
        if(obj->isWidgetType())
        {
            QString name = obj->metaObject()->className();
            if(name=="QComboBox"){
                QComboBox* combo = (QComboBox*)obj;
                QString key = combo->accessibleName();
                if(key!=""){
                    if(map->contains(key))
                    {
                        QString value = map->value(key);
                        //QString value = map[key];
                        int index=0;
                        for(int i=0; i<combo->count();i++)
                        {
                            if(combo->itemText(i)==value)
                            {
                                index = i;
                                break;
                            }
                        }
                        combo->setCurrentIndex(index);
                    }
                }
            }
            else if(name=="QTextEdit"){
                QTextEdit* textEdit = (QTextEdit*)obj;
                QString key = textEdit->accessibleName();
                if(key!=""){
                    if(map->contains(key))
                    {
                        textEdit->setText(map->value(key));
                    }
                }
            }
            else if(name=="QLineEdit"){
                QLineEdit* lineEdit = (QLineEdit*)obj;
                QString key = lineEdit->accessibleName();
                if(key!=""){
                    if(map->contains(key))
                    {
                        lineEdit->setText(map->value(key));
                    }
                }
            }
            else if(name=="QLabel"){
                QLabel* label = (QLabel*)obj;
                QString key = label->accessibleName();
                if(key!=""){
                    if(map->contains(key))
                    {
                        label->setText(map->value(key));
                    }
                }
            }
            else if(name=="QListWidget"){

                QListWidget* listWidget = (QListWidget*)obj;
                if(listWidget->accessibleName()=="sim_regions")// This is hardcoded for regions, making this generic would be too complicated
                {
                    //get all keys from map that start with sim_region_number_
                    //QMap<QString, QString> regions();
                    foreach(QString key, map->keys())
                    {
                        if(key.startsWith("sim_region_number_"))
                        {
                            QString regionData = map->value(key);
                            regionData = ConfigHelper::RemoveQuotes(regionData);
                            QStringList pairs = regionData.split("|");
                            QString name = "";
                            foreach(QString pair, pairs)
                            {
                                if(pair.split("=").at(0)=="region_name")
                                name = pair.split("=").at(1);
                            }
                            //regions.insert(key,map->value(key));
                            QListWidgetItem* item = new QListWidgetItem(listWidget);
                            name = RemoveQuotes(name);
                            item->setText(name);
                            item->setToolTip(regionData);
                            listWidget->insertItem(0,item);
                            //*/
                        }
                    }

                }
                //*/
            }
            else{
                //QWidget* w = qobject_cast<QWidget*>(obj);
                SetValuesToUI(map, (QWidget*)obj);
            }

        }
    }
    //*/
}

QString ConfigHelper::SetQuotes(QString string)
{
    if(!string.startsWith("\"")&&!string.endsWith("\"")){
        string.insert(0,"\"");
        string.append("\"");
    }
    return string;
}

QString ConfigHelper::RemoveQuotes(QString string)
{
    if(string.startsWith("\"")&&string.endsWith("\"")){
        string = string.mid(1,string.length()-2);
    }
    if(string.startsWith("\"")&&!string.endsWith("\"")){
        string = string.mid(1,string.length()-1);
    }
    if(!string.startsWith("\"")&&string.endsWith("\"")){
        string = string.mid(0,string.length()-1);
    }
    return string;
}

bool ConfigHelper::CheckIfFileExists(QString path)
{
     QFile f( path );
      if( !f.exists() )
      {
        return false;
      }
      return true;
}


void ConfigHelper::SetFontToWidgetRecursively(QFont& font, QWidget* widget)
{
    QObjectList children = widget->children();
    foreach(QObject* obj, children)
    {
        if(obj->isWidgetType())
        {
            QWidget* widgetObj = (QWidget*)obj;
            SetFontToWidgetRecursively(font, widgetObj);
        }
    }
    widget->setFont(font);
}
