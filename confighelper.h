#ifndef CONFIGHELPER_H
#define CONFIGHELPER_H
//#include <QString>
#include <QUrl>
#include <QFormLayout>
#include <QMap>
#include <QListWidget>
#include <QFont>

enum FileDialogType
{
    saveFile,
    openFile,
    selectFolder
};


class ConfigHelper
{
public:
    ConfigHelper();


    static QString SelectPathOrFolder(QString caption, QWidget* parent, QString dir, FileDialogType type, QString filter="");

    // return "" if all urls are valid, else name of invalid url
    static QString CheckURLsFromMap(QMap<QString,QString> map);

    // convert formlayot to qmap with some predesided thingies
    // reserves memory to map, that must be released
    static QMap<QString,QString>* FormLayoutToMap(QFormLayout* formLayout);

    static void MapToFormLayout(QMap<QString,QString>* map, QFormLayout* formLayout);
    static void MapToFormLayout(QMap<QString,QString>* map, QGridLayout* formLayout);

    static void CollectAllTaggedValuesFromPage(QMap<QString,QString>* map, QWidget* widget);    
    static void CopyMapToMap(QMap<QString,QString>* destiny, QMap<QString,QString>* source);

    static QString CheckRegionForConflicts(QMap<QString, QMap<QString,QString>* > regions, QMap<QString,QString>* newRegion);
    static QString CheckRegionForConflicts(QListWidget* regions, QMap<QString,QString>* newRegion);

    static QString RegionMapToString(QMap<QString,QString>* map);
    static QMap<QString, QMap<QString,QString>* > ListWidgetToMap(QListWidget* listWidget);

    static QMap<QString,QString>* StringToMap(QString str, QString separator, QString pairSeparator);

    static void SetConfigValuesToPage(QMap<QString,QMap<QString,QString>* > sections, QWidget* widget);

    static QString SetQuotes(QString string);
    static QString RemoveQuotes(QString string);

    static bool CheckIfFileExists(QString path);

    static void SetFontToWidgetRecursively(QFont& font, QWidget* widget);

private:

    static QMap<QString,QString>* ConfigSectionsToValueMap(QMap<QString,QMap<QString,QString>* > sections);

    // returns region widget
    static void SetValuesToUI(QMap<QString,QString>* map, QWidget* widget);



};

#endif // CONFIGHELPER_H
