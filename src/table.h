#ifndef LBDATABASE_TABLE_H
#define LBDATABASE_TABLE_H

#include <QAbstractTableModel>

#include "databaseattributemetadata.h"

#include <QMetaType>
#include <QSqlQuery>

#include <QDebug>

namespace LBDatabase {

class Attribute;
class Database;
class Row;
class TableMetaData;

class Table : public QAbstractTableModel
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name)

public:
    QString name() const;
    Database *database() const;
    TableMetaData *metaData() const;

    Row *rowById(int id) const;

    Row *rowByModelIndex(const QModelIndex &index);

    void addDatabaseAttribute(const QString &name, const QString &displayName, DatabaseAttributeMetaData::SqlType type);

    // QAbstractTableModel
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    void removeDatabaseAttribute(const QString &name);

protected slots:
    void attributeMetaDataDisplayNameChanged();

protected:
    friend class Database;
    friend class DatabaseAttributeMetaData;
    friend class AttributeMetaData;

    Table();
    Table(const QString &name, Database *database);
    void initWith(const QString &name, Database *database);

    virtual Row* createRowInstance(int id, const QSqlQuery &query);

    void changeColumnName(const QString &name, const QString &newName);

    TableMetaData *m_metaData;

    QHash<int, Row *> m_rowsById;
    QList<Row *> m_rows;

    Database *m_database;
    QString m_name;
};

template<class TableClass>
class lbQRegisterMetaTypeHelper
{
public:
    lbQRegisterMetaTypeHelper(QString) // i have no idea, why this shouldnt work without the QString...
    {
        qRegisterMetaType<TableClass>();
    }
};

} // namespace LBDatabase

Q_DECLARE_METATYPE(LBDatabase::Table*)


#define STRINGIZE(s) # s
#define DATABASE
#define LB_TABLE(classname) \
    public: \
    classname(); \
    classname(::LBDatabase::Database*); \
    classname(const classname &);

#define LB_REGISTER_TABLE(classname) \
    classname::classname(::LBDatabase::Database* database) : \
    ::LBDatabase::Table(QLatin1String(STRINGIZE(classname) ""), \
                        database) \
    {} \
    classname::classname() : \
    ::LBDatabase::Table() \
    {} \
    classname::classname(const Companies &) : \
    ::LBDatabase::Table(QLatin1String(STRINGIZE(classname) ""), \
                        ::LBDatabase::Database::instance(QLatin1String(STRINGIZE(databaseName) ""))) \
    { \
    qWarning() << "You should never call the copy constructor of a table!"; \
    } \
    Q_DECLARE_METATYPE(classname) \
    ::LBDatabase::lbQRegisterMetaTypeHelper<classname> _register_ ## classname(QLatin1String(STRINGIZE(databaseName) ""));

#endif // LBDATABASE_TABLE_H
