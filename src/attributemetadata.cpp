#include "attributemetadata.h"

#include "attribute.h"
#include "database.h"
#include "databaseattribute.h"
#include "databaseattributemetadata.h"
#include "table.h"
#include "tablemetadata.h"

#include <QSqlRecord>

#include <QSqlError>
#include <QDebug>

namespace LBDatabase {

AttributeMetaData::AttributeMetaData(const QSqlQuery &query, Table *table, TableMetaData *metaData) :
    QObject(metaData),
    m_table(table),
    m_name(QString()),
    m_displayName(QString()),
    m_type(UnknownType),
    m_isEditable(false)
{
    QSqlRecord record = query.record();
    int nameIndex = record.indexOf(QLatin1String("name"));
    int displayNameIndex = record.indexOf(QLatin1String("displayName"));
    int typeIndex = record.indexOf(QLatin1String("type"));

    m_name = query.value(nameIndex).toString();
    m_displayName = query.value(displayNameIndex).toString();
    m_type = static_cast<Type>(query.value(typeIndex).toInt());

    metaData->m_attributeMetaDataByName.insert(m_name, this);
    metaData->beginInsertRows(QModelIndex(),metaData->m_attributeMetaDatas.size(),metaData->m_attributeMetaDatas.size());
    metaData->m_attributeMetaDatas.append(this);
    metaData->endInsertRows();

    connect(this, SIGNAL(nameChanged()), metaData, SLOT(attributeMetaDataNameChanged()));
    connect(this, SIGNAL(displayNameChanged()), metaData, SLOT(attributeMetaDataDisplayNameChanged()));
}

QString AttributeMetaData::name() const
{
    return m_name;
}

QString AttributeMetaData::displayName() const
{
    return m_displayName;
}

bool AttributeMetaData::isEditable() const
{
    return m_isEditable;
}

AttributeMetaData::Type AttributeMetaData::type() const
{
    return m_type;
}

QString AttributeMetaData::typeName() const
{
    return AttributeMetaData::typeToName(m_type);
}

QString AttributeMetaData::typeToName(Type type)
{
    switch(type) {
    case UnknownType:
        return tr("Unknown");
    case DatabaseAttributeType:
        return tr("Database Attribute");
    }

    return tr("Invalid");
}

Attribute *AttributeMetaData::createAttributeInstance(Row *row)
{
    switch(m_type) {
    case UnknownType:
        Q_ASSERT_X(false, "AttributeMetaData::createAttributeInstance", "Cannot create attribute instance of unknown type.");
    case DatabaseAttributeType:
        return new DatabaseAttribute(m_name, row);
    }

    Q_ASSERT_X(false, "AttributeMetaData::createAttributeInstance", "Cannot create attribute instance of invalid type.");
    return 0;
}

void AttributeMetaData::setName(const QString &name)
{
    QSqlQuery query(m_table->database()->sqlDatabase());
    query.exec(QLatin1String("UPDATE ")+m_table->name()+
               QLatin1String("_lbmeta_attributes SET name = '")+name+QLatin1String("' WHERE name = '")+m_name+QLatin1String("';"));
    query.finish();
    m_name = name;
    emit nameChanged();
}

void AttributeMetaData::setDisplayName(const QString &name)
{
    QSqlQuery query(m_table->database()->sqlDatabase());
    query.exec("UPDATE "+m_table->name()+"_lbmeta_attributes SET displayName = '"+name+"' WHERE name = '"+m_name+"'");
    query.finish();
    m_displayName = name;
    emit displayNameChanged();
}

AttributeMetaData *AttributeMetaData::createMetaDataInstance(const QSqlQuery &query, Table *table, TableMetaData *metaData)
{
    int typeIndex = query.record().indexOf(QLatin1String("type"));

    switch(static_cast<Type>(query.value(typeIndex).toInt())) {
    case DatabaseAttributeType:
        return new DatabaseAttributeMetaData(query, table, metaData);
    case UnknownType:
        Q_ASSERT_X(false, "AttributeMetaData::createMetaDataInstance", "Cannot create meta data instance of unkown type.");
    default:
        break;
    }

    Q_ASSERT_X(false, "AttributeMetaData::createMetaDataInstance", "Cannot create meta data instance of invalid type.");
    return 0;
}

} // namespace LBDatabase
