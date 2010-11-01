#ifndef CONFIGFILE_H
#define CONFIGFILE_H

#include <QList>
#include <QFile>

class ConfigFile : public QObject
{
public:
    ConfigFile();
    ConfigFile(QString fileName);
    bool loadSuccess;
    bool saveSuccess;


    void LoadConfig();
    QFile* OpenFile(QString filename);
    QFile* OpenFileForWriting(QString filename);
    void SaveFile(QString path="");
    void SaveFileWithAbsolutePath(QString path);

    QString ConfigFileName;
    QString ConfigContents;
    QString ConfigType;

private:
    void DeleteOldFileIfExists(QString filename);
};

#endif // CONFIGFILE_H
