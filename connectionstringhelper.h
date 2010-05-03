#ifndef CONNECTIONSTRINGHELPER_H
#define CONNECTIONSTRINGHELPER_H

#include <QString>

class ConnectionStringHelper
{
public:
    ConnectionStringHelper();

    static QString m_MYSQL_ConnectionString;
    static QString m_SQLITE_ConnectionString;
    static QString m_NHIBERNATE_ConnectionString;

    static QString ConstructMySqlConnectionString(QString dataSource, QString user, QString password);
    static QString ConstructSqliteConnectionString();
    static QString ConstructNHibernateConnectionString(QString dataSource, QString user, QString password, QString dbPlugin, QString& propertyProvider);

    static void DBParamsFromConnectionString(QString dbPlugin, QString connStr, QString& user, QString& password, QString& datasource);
};

#endif // CONNECTIONSTRINGHELPER_H
