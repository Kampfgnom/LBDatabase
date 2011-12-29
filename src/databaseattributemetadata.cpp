#include "databaseattributemetadata.h"

#include "table.h"
#include "tablemetadata.h"

#include <QSqlRecord>

namespace LBDatabase {

DatabaseAttributeMetaData::DatabaseAttributeMetaData(const QSqlQuery &query, Table *table, TableMetaData *metaData) :
    AttributeMetaData(query, table, metaData)
{
    QSqlRecord record = query.record();
    int sqlTypeIndex = record.indexOf(QLatin1String("sqlType"));

    m_sqlType = static_cast<SqlType>(query.value(sqlTypeIndex).toInt());
    m_isEditable = true;

    metaData->m_databaseAttributeMetaDatas.append(this);
}

void DatabaseAttributeMetaData::setName(const QString &name)
{
    m_table->changeColumnName(m_name, name);
    AttributeMetaData::setName(name);
}

DatabaseAttributeMetaData::SqlType DatabaseAttributeMetaData::sqlType() const
{
    return m_sqlType;
}

QString DatabaseAttributeMetaData::sqlTypeName() const
{
    return sqlTypeToName(m_sqlType);
}

QString DatabaseAttributeMetaData::sqlTypeToName(SqlType type)
{
    switch(type) {
    case DateSqlType:
        return "DATETIME";
        break;
    case DoubleSqlType:
        return "DOUBLE";
        break;
    case IntegerSqlType:
        return "INTEGER";
        break;
    case TextSqlType:
        return "TEXT";
        break;
    default:
        break;
    }

    qWarning() << "DatabaseAttributeMetaData::sqlTypeToName: Unkown type!";
    return QString();
}

QStringList DatabaseAttributeMetaData::sqlTypeNames()
{
    static QStringList names;
    static bool first = true;
    if(first) {
        first = false;
        names << sqlTypeToName(IntegerSqlType) <<
                 sqlTypeToName(DateSqlType) <<
                 sqlTypeToName(DoubleSqlType) <<
                 sqlTypeToName(TextSqlType);
    }
    return names;
}

} // namespace LBDatabase
