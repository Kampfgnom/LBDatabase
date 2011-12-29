#include "tablemetadata.h"

#include "attributemetadata.h"
#include "database.h"
#include "databaseattributemetadata.h"
#include "table.h"

#include <QSqlDatabase>
#include <QSqlRecord>

namespace LBDatabase {

TableMetaData::TableMetaData(Table *parent) :
    QAbstractTableModel(parent),
    m_table(parent),
    m_attributeMetaDataByName(QHash<QString, AttributeMetaData *>()),
    m_attributeMetaDatas(QList<AttributeMetaData *>()),
    m_databaseAttributeMetaDatas(QList<DatabaseAttributeMetaData *>())
{
    QSqlDatabase sqlDatabase = m_table->database()->sqlDatabase();
    QSqlQuery query(sqlDatabase);
    query.exec(QLatin1String("SELECT * FROM ")+m_table->name()+QLatin1String("_lbmeta_attributes"));
    m_attributeMetaDataByName.reserve(query.size());
    m_attributeMetaDatas.reserve(query.size());

    while(query.next()) {
        AttributeMetaData::createMetaDataInstance(query, m_table, this);
    }
}

AttributeMetaData *TableMetaData::attributeMetaData(const QString &name)
{
    return m_attributeMetaDataByName.value(name, 0);
}

QList<AttributeMetaData *> TableMetaData::attributeMetaDatas()
{
    return m_attributeMetaDatas;
}

QList<DatabaseAttributeMetaData *> TableMetaData::databaseAttributeMetaDatas()
{
    return m_databaseAttributeMetaDatas;
}

int TableMetaData::attributeCount() const
{
    return m_attributeMetaDatas.size();
}

QVariant TableMetaData::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DisplayRole || role == Qt::EditRole) {
        AttributeMetaData *data = m_attributeMetaDatas.at(index.row());
        switch(index.column()) {
        case 0:
            return data->name();
        case 1:
            return data->displayName();
        case 2:
            return data->typeName();
        case 3:
            if(data->type() == AttributeMetaData::DatabaseAttributeType) {
                return static_cast<DatabaseAttributeMetaData *>(data)->sqlTypeName();
            }
        }
    }

    return QVariant();
}

QVariant TableMetaData::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal) {
        if(role == Qt::DisplayRole) {
            switch(section) {
            case 0:
                return tr("Name");
            case 1:
                return tr("Display Name");
            case 2:
                return tr("Type");
            case 3:
                return tr("SQL Type");
            }
        }
        else if(role == Qt::TextAlignmentRole) {
            return Qt::AlignLeft;
        }
    }
    return QVariant();
}

int TableMetaData::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid()) {
        return 0;
    }
    return 4;
}

int TableMetaData::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid()) {
        return 0;
    }
    return m_attributeMetaDatas.size();
}

void TableMetaData::attributeMetaDataNameChanged()
{
    AttributeMetaData *metaData = static_cast<AttributeMetaData *>(sender());
    m_attributeMetaDataByName.remove(metaData->name());
    m_attributeMetaDataByName.insert(metaData->name(),metaData);
    QModelIndex i = index(m_attributeMetaDatas.indexOf(metaData),0);
    emit dataChanged(i,i);
}

void TableMetaData::attributeMetaDataDisplayNameChanged()
{
    AttributeMetaData *metaData = static_cast<AttributeMetaData *>(sender());
    QModelIndex i = index(m_attributeMetaDatas.indexOf(metaData),1);
    emit dataChanged(i,i);
}

AttributeMetaData *TableMetaData::attributeMetaDataByModelIndex(const QModelIndex &index)
{
    return m_attributeMetaDatas.at(index.row());
}

} // namespace LBDatabase
