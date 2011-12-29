#include "table.h"

#include "attributemetadata.h"
#include "database.h"
#include "databaseattribute.h"
#include "databaseattributemetadata.h"
#include "row.h"
#include "tablemetadata.h"

#include <QSqlRecord>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlError>

namespace LBDatabase {

Table::Table() :
    QAbstractTableModel(),
    m_metaData(0),
    m_rowsById(QHash<int, Row *>()),
    m_rows(QList<Row *>()),
    m_database(0),
    m_name(QString())
{
}

Table::Table(const QString &name, Database *database) :
    QAbstractTableModel(database),
    m_metaData(0),
    m_rowsById(QHash<int, Row *>()),
    m_rows(QList<Row *>()),
    m_database(database),
    m_name(name)
{
    initWith(name, database);
}

void Table::initWith(const QString &name, Database *database)
{
    m_name = name;
    m_database = database;
    setParent(database);

    m_metaData = new TableMetaData(this);

    foreach(DatabaseAttributeMetaData *d, m_metaData->databaseAttributeMetaDatas()) {
        connect(d, SIGNAL(displayNameChanged()), this, SLOT(attributeMetaDataDisplayNameChanged()));
    }

    QSqlQuery query(m_database->sqlDatabase());
    query.exec(QLatin1String("SELECT * FROM ")+m_name);
    QSqlRecord record = query.record();
    int idIndex = record.indexOf(QLatin1String("id"));
    QHash<AttributeMetaData *, int> attributeIds;
    foreach(AttributeMetaData *d, m_metaData->databaseAttributeMetaDatas()) {
        attributeIds.insert(d, record.indexOf(d->name()));
    }

    m_rowsById.reserve(query.size());
    m_rows.reserve(query.size());

    int id = 0;
    while(query.next()) {
        id = query.value(idIndex).toInt();
        Row *row = createRowInstance(id, query);
        foreach(DatabaseAttribute *databaseAttribute, row->databaseAttributes()) {
            databaseAttribute->initWithValue(query.value(attributeIds.value(databaseAttribute->metaData())));
        }

        m_rowsById.insert(id, row);
        m_rows.append(row);
    }
    query.finish();
}

Row *Table::createRowInstance(int id, const QSqlQuery &query)
{
    int typeIndex = query.record().indexOf(QLatin1String("lbmeta_type"));
    int metaTypeId = 0;
    if(typeIndex > -1) {
        QString name = query.value(typeIndex).toString();
        metaTypeId = QMetaType::type(name.toUtf8().constData());
    }

    if(!metaTypeId) {
        return new Row(id, this);
    }
    else {
        QObject *object = static_cast<QObject*>(QMetaType::construct(metaTypeId));
        Row *row = qobject_cast<Row*>(object);
        if(row) {
            row->initWith(id, this);
            return row;
        }
        else {
            delete object;
            return new Row(id, this);
        }
    }
}

QString Table::name() const
{
    return m_name;
}

Database *Table::database() const
{
    return m_database;
}

TableMetaData *Table::metaData() const
{
    return m_metaData;
}

Row *Table::rowById(int id) const
{
    return m_rowsById.value(id);
}

Row *Table::rowByModelIndex(const QModelIndex &index)
{
    return m_rows.at(index.row());
}

void Table::changeColumnName(const QString &name, const QString &newName)
{
    QSqlQuery query(m_database->sqlDatabase());
    QString create = QLatin1String("CREATE TABLE ")+m_name+QLatin1String("_tmp AS SELECT ");
    foreach(AttributeMetaData *attribute, m_metaData->attributeMetaDatas()) {
        if(attribute->name() != name) {
            create += attribute->name()+QLatin1String(",");
        }
    }
    create += name+QLatin1String(" AS ")+newName+QLatin1String(" FROM ")+m_name+QLatin1String(";");

    query.exec(QLatin1String("BEGIN TRANSACTION;"));
    query.exec(create);
    query.exec(QLatin1String("DROP TABLE ")+m_name);
    query.exec(QLatin1String("ALTER TABLE ")+m_name+QLatin1String("_tmp RENAME TO ")+m_name);
    query.exec(QLatin1String("COMMIT;"));
    query.finish();
}

void Table::removeDatabaseAttribute(const QString &name)
{
    DatabaseAttributeMetaData *metaData = static_cast<DatabaseAttributeMetaData *>(m_metaData->attributeMetaData(name));
    beginRemoveColumns(QModelIndex(),
                       m_metaData->m_attributeMetaDatas.indexOf(metaData),m_metaData->m_attributeMetaDatas.indexOf(metaData));
    m_metaData->beginRemoveRows(QModelIndex(),
                                m_metaData->m_attributeMetaDatas.indexOf(metaData),m_metaData->m_attributeMetaDatas.indexOf(metaData));
    m_metaData->m_databaseAttributeMetaDatas.removeAll(metaData);
    m_metaData->m_attributeMetaDatas.removeAll(metaData);
    m_metaData->m_attributeMetaDataByName.remove(name);
    foreach(Row *row, m_rows) {
        Attribute *attribute = row->attribute(name);
        row->m_attributes.removeAll(attribute);
        row->m_databaseAttributes.removeAll(static_cast<DatabaseAttribute *>(attribute));
        attribute->deleteLater();
    }
    m_metaData->endRemoveRows();

    QSqlQuery query(m_database->sqlDatabase());
    QString create = QLatin1String("CREATE TABLE ")+m_name+QLatin1String("_tmp AS SELECT ");
    foreach(AttributeMetaData *attribute, m_metaData->attributeMetaDatas()) {
        if(attribute->name() != name) {
            create += attribute->name()+QLatin1String(",");
        }
    }
    create.remove(create.lastIndexOf(','),1);
    create += QLatin1String(" FROM ")+m_name+QLatin1String(";");

    query.exec(QLatin1String("BEGIN TRANSACTION;"));
    query.exec(create);
    query.exec(QLatin1String("DROP TABLE ")+m_name);
    query.exec(QLatin1String("ALTER TABLE ")+m_name+QLatin1String("_tmp RENAME TO ")+m_name);
    query.exec(QLatin1String("DELETE FROM ")+m_name+QLatin1String("_lbmeta_attributes WHERE name = '")+name+QLatin1String("';"));
    query.exec(QLatin1String("COMMIT;"));
    query.finish();

    endRemoveColumns();
    metaData->deleteLater();
}

void Table::addDatabaseAttribute(const QString &name, const QString &displayName, DatabaseAttributeMetaData::SqlType type)
{
    QSqlQuery query(m_database->sqlDatabase());

    query.exec(QLatin1String("ALTER TABLE ")+m_name+QLatin1String(" ADD ")+name+
               QLatin1String(" ")+DatabaseAttributeMetaData::sqlTypeToName(type)+QLatin1String(";"));

    query.exec(QLatin1String("INSERT INTO ")+m_name+
               QLatin1String("_lbmeta_attributes (name, displayName, sqlType, type) VALUES ('")
               +name+QLatin1String("','")
               +displayName+QLatin1String("','")
               +QString::number(type)+QLatin1String("','")
               +QString::number(AttributeMetaData::DatabaseAttributeType)
               +QLatin1String("');"));

    query.exec(QLatin1String("SELECT * FROM ")+m_name+
               QLatin1String("_lbmeta_attributes WHERE id = '")+query.lastInsertId().toString()+QLatin1String("';"));
    query.first();

    beginInsertColumns(QModelIndex(), m_metaData->attributeCount(), m_metaData->attributeCount());
    AttributeMetaData *metaData = AttributeMetaData::createMetaDataInstance(query, this, this->metaData());
    foreach(Row *row, m_rows) {
        metaData->createAttributeInstance(row);
    }
    endInsertColumns();

    query.finish();
}

QVariant Table::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DisplayRole || role == Qt::EditRole) {
        Row *row = m_rows.at(index.row());
        return row->attributes().at(index.column())->data();
    }

    return QVariant();
}

QVariant Table::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal) {
        if(role == Qt::DisplayRole) {
            return m_metaData->attributeMetaDatas().at(section)->displayName();
        }
        else if(role == Qt::TextAlignmentRole) {
            return Qt::AlignLeft;
        }
    }
    return QVariant();
}

int Table::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid() || !m_metaData) {
        return 0;
    }
    return m_metaData->attributeCount();
}

int Table::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid()) {
        return 0;
    }
    return m_rows.size();
}

bool Table::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role == Qt::EditRole) {
        Row *row = m_rows.at(index.row());
        Attribute *attribute = row->attributes().at(index.column());
        if(attribute->isEditable()) {
            attribute->setData(value);
            return true;
        }
    }
    return false;
}

Qt::ItemFlags Table::flags(const QModelIndex &index) const
{
    Row *row = m_rows.at(index.row());
    Attribute *attribute = row->attributes().at(index.column());

    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);
    if(attribute->isEditable()) {
        defaultFlags |= Qt::ItemIsEditable;
    }

    return defaultFlags;
}

void Table::attributeMetaDataDisplayNameChanged()
{
    AttributeMetaData *metaData = static_cast<AttributeMetaData *>(sender());
    int i = m_metaData->attributeMetaDatas().indexOf(metaData);
    emit headerDataChanged(Qt::Horizontal,i,i);
}

} // namespace LBDatabase
