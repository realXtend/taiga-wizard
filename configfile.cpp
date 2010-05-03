#include "configfile.h"
#include <QTextStream>
#include <QDir>
#include <iostream>
#include <fstream>
#include <string>

ConfigFile::ConfigFile()
{

}

ConfigFile::ConfigFile(QString fileName)
{
    ConfigFileName = fileName;
    LoadConfig();
}

void ConfigFile::LoadConfig()
{
    if(ConfigFileName.length()==0){return;} // name must be set first

    // load file
    QFile *file = OpenFile(this->ConfigFileName);
    if (!file){
        loadSuccess = false;
        return;
    }
    //QByteArray content = file->readAll();
    QTextStream stream(file);
    QString content;
    while(!stream.atEnd())
        content += stream.readLine() + '\n';
    ConfigContents = content;
    file->close();

    QString str = stream.readAll();
    loadSuccess = true;
}

QFile *ConfigFile::OpenFile(QString filename)
{
    QFile *file = new QFile(filename);
    if (!file->exists())
        return 0;
    if (!file->open(QIODevice::ReadOnly|QIODevice::Text))
        return 0;

    return file;
}

QFile* ConfigFile::OpenFileForWriting(QString filename)
{
    QFile *file = new QFile(filename);
    if (!file->open(QIODevice::ReadWrite|QIODevice::Text))
        return 0;
    return file;

    /*
    if (!file->exists())
        return 0;
    if (!file->open(QIODevice::WriteOnly|QIODevice::Text))
        return 0;
    return file;
    //*/
}

void ConfigFile::SaveFile(QString path)
{
    QString current = QDir::currentPath();
    QString destinyFile;
    QStringList list = ConfigFileName.split(QDir::separator());
    QString fileName = list.operator [](list.count()-1);
    if(path!=""){
        //destinyFile = current + QDir::separator() + path + QDir::separator() + fileName;
        destinyFile = path + QDir::separator() + fileName;
    } else {
        destinyFile = current + QDir::separator() + fileName;
    }
    destinyFile = destinyFile.replace("/", QDir::separator());
    destinyFile = destinyFile.replace("\\", QDir::separator());

    DeleteOldFileIfExists(destinyFile);
    QFile *file = OpenFileForWriting(destinyFile);
    if (!file){
        saveSuccess = false;
        return;
    }

    std::string str = this->ConfigContents.toStdString();
    file->write(str.c_str());
    file->flush();
    file->close();
    saveSuccess=true;
}

void ConfigFile::SaveFileWithAbsolutePath(QString path)
{
    path = path.replace("/", QDir::separator());
    path = path.replace("\\", QDir::separator());

    DeleteOldFileIfExists(path);

    QFile *file = OpenFileForWriting(path);
    if (!file){
        saveSuccess = false;
        return;
    }

    std::string str = this->ConfigContents.toStdString();
    file->write(str.c_str());
    file->flush();
    file->close();
    saveSuccess=true;
}

void ConfigFile::DeleteOldFileIfExists(QString filename)
{
    QFile *file = new QFile(filename);
    if (!file->open(QIODevice::ReadWrite|QIODevice::Text))
        return;
    file->remove();
    file->close();
}

