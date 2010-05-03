#include "connectionstringhelper.h"
#include <QString>
#include "confighelper.h"

QString ConnectionStringHelper::m_MYSQL_ConnectionString = "Data Source=${DataSource}$;Database=${Database}$;User ID=${User}$;Password=${Password}$;";
//QString ConnectionStringHelper::m_SQLITE_ConnectionString = "URI=file:OpenSim.db,version=3";
QString ConnectionStringHelper::m_SQLITE_ConnectionString = "URI=file:${Database}$.db,version=3";
QString ConnectionStringHelper::m_NHIBERNATE_ConnectionString = "MySQLDialect;MySqlDataDriver;Data Source=${DataSource}$;Database=${Database}$;User ID=${User}$;Password=${Password}$";


ConnectionStringHelper::ConnectionStringHelper()
{
}

QString ConnectionStringHelper::ConstructMySqlConnectionString(QString dataSource, QString user, QString password)
{
    QString connectionString = m_MYSQL_ConnectionString;
    connectionString = connectionString.replace("${DataSource}$", dataSource);
    //connectionString = connectionString.replace("${Database}$", database); // replace later
    connectionString = connectionString.replace("${User}$", user);
    connectionString = connectionString.replace("${Password}$", password);
    return connectionString;
}

QString ConnectionStringHelper::ConstructSqliteConnectionString()
{
    QString connectionString = m_SQLITE_ConnectionString;
    //connectionString = connectionString.replace()
    return connectionString;
}

QString ConnectionStringHelper::ConstructNHibernateConnectionString(QString dataSource, QString user, QString password, QString dbPlugin, QString& propertyProvider)
{
    if(dbPlugin=="OpenSim.Data.MySQL.dll")
    {
        propertyProvider = "NHibernatePropertyStorage";
    }
    else
    {
        propertyProvider = "DummyProperyProvider";
    }
    QString connectionString = m_NHIBERNATE_ConnectionString;
    connectionString = connectionString.replace("${DataSource}$", dataSource);
    //connectionString = connectionString.replace("${Database}$", database); // replace later
    connectionString = connectionString.replace("${User}$", user);
    connectionString = connectionString.replace("${Password}$", password);
    return connectionString;
}


void ConnectionStringHelper::DBParamsFromConnectionString(QString dbPlugin, QString connStr, QString& user, QString& password, QString& datasource)
{
    /*
    QString ConnectionStringHelper::m_MYSQL_ConnectionString = "Data Source=${DataSource}$;Database=${Database}$;User ID=${User}$;Password=${Password}$;";
    //QString ConnectionStringHelper::m_SQLITE_ConnectionString = "URI=file:OpenSim.db,version=3";
    QString ConnectionStringHelper::m_SQLITE_ConnectionString = "URI=file:${Database}$.db,version=3";
    QString ConnectionStringHelper::m_NHIBERNATE_ConnectionString = "MySQLDialect;MySqlDataDriver;Data Source=${DataSource}$;Database=${Database}$;User ID=${User}$;Password=${Password}$";
    //*/
    if(dbPlugin=="OpenSim.Data.MySQL.dll")
    {
        QMap<QString,QString>* map = ConfigHelper::StringToMap(connStr, ";", "=");
        datasource = map->operator []("Data Source");
        user = map->operator []("User ID");
        password = map->operator []("Password");
    }
    else if(dbPlugin=="OpenSim.Data.SQLite.dll")
    {
        // nothing to do
    }
}
