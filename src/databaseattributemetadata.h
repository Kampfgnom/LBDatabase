#ifndef LBDATABASE_DATABASEATTRIBUTEMETADATA_H
#define LBDATABASE_DATABASEATTRIBUTEMETADATA_H

#include "attributemetadata.h"

namespace LBDatabase {

class DatabaseAttributeMetaData : public AttributeMetaData
{
    Q_OBJECT
    Q_PROPERTY(SqlType sqlType READ sqlType)
    Q_PROPERTY(QString sqlTypeName READ sqlTypeName)
public:
    enum SqlType {
        UnknownType,
        DateSqlType,
        DoubleSqlType,
        IntegerSqlType,
        TextSqlType
    };

    void setName(const QString &name);

    SqlType sqlType() const;

    QString sqlTypeName() const;

    static QString sqlTypeToName(SqlType type);

    static QStringList sqlTypeNames();

private:
    friend class AttributeMetaData;

    explicit DatabaseAttributeMetaData(const QSqlQuery &query, Table *table, TableMetaData *metaData);

    SqlType m_sqlType;
};

} // namespace LBDatabase

#endif // LBDATABASE_DATABASEATTRIBUTEMETADATA_H
