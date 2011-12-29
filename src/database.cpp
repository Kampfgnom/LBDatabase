#include "database.h"

#include "table.h"

#include <QMutexLocker>
#include <QSqlError>
#include <QStringList>
#include <QMetaType>

#include <QDebug>

namespace LBDatabase {

/*
  Static
  */
QHash<QString, Database*> Database::s_instances = QHash<QString, Database*>();

Database *Database::instance(const QString &name, const QString &fileName)
{
    static QMutex mutex(QMutex::Recursive);
    static QObject guard;
    QMutexLocker locker(&mutex); (void) locker;

    if(s_instances.contains(fileName)) {
        return s_instances.value(fileName);
    }

    Database* database = new Database(name, fileName, &guard);
    s_instances.insert(fileName, database);
    database->initialize();
    return database;
}

Database *Database::instance(const QString &fileName)
{
    return Database::instance(fileName, fileName);
}

QList<Database *> Database::instances()
{
    return s_instances.values();
}

/*
  Members
  */
Database::Database(const QString &name, const QString &fileName, QObject *parent) :
    QObject(parent),
    m_name(name),
    m_file(fileName),
    m_tables(QMap<QString, Table*>())
{
    qDebug() << "Database::Database: Opening database file" << fileName;

    QSqlDatabase sqlDatabase = QSqlDatabase::addDatabase("QSQLITE", fileName);
    sqlDatabase.setDatabaseName(fileName);
    sqlDatabase.setUserName("projectstats");
    sqlDatabase.setPassword("projectstats");
    if(!sqlDatabase.open()) {
        qCritical() << "Database::loadFile: Could not open database file" << fileName;
        qCritical() << "Database::loadFile: " << sqlDatabase.lastError();
    }
}

void Database::initialize()
{
    createTableInstances();
}

void Database::createTableInstances()
{
    foreach(QString tableName, sqlDatabase().tables()) {
        if(!tableName.endsWith(QLatin1String("_lbmeta_attributes"))) {
            Table *table = createTableInstace(tableName);
            m_tables.insert(tableName, table);
        }
    }
}

Table *Database::createTableInstace(const QString &name)
{
    int metaTypeId = QMetaType::type(name.toUtf8().constData());

    if(!metaTypeId) {
        qDebug() << "Database::createTableInstace: No class for table" << name << "found. Creating general table instance.";
        return new Table(name, this);
    }
    else {
        QObject *object = static_cast<QObject*>(QMetaType::construct(metaTypeId));
        Table *table = qobject_cast<Table*>(object);
        if(table) {
            qDebug() << "Database::createTableInstace: Creating table class" << name << ".";
            table->initWith(name, this);
            return table;
        }
        else {
            qDebug() << "Database::createTableInstace: Class" << name << "is no table class. Creating general table instance.";
            delete object;
            return new Table(name, this);
        }
    }
}

Database::~Database()
{
    qDebug() << "Database::Database: Closing database file" << m_file.fileName();
}

QString Database::name() const
{
    return m_name;
}

QList<Table *> Database::tables() const
{
    return m_tables.values();
}

Table *Database::table(const QString &name) const
{
    return m_tables.value(name, 0);
}

QSqlDatabase Database::sqlDatabase() const
{
    return QSqlDatabase::database(m_file.fileName());
}

QString Database::fileName() const
{
    return m_file.fileName();
}

} // namespace LBDatabase
