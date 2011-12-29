#ifndef LBDATABASE_DATABASEATTRIBUTE_H
#define LBDATABASE_DATABASEATTRIBUTE_H

#include "attribute.h"
#include <QSqlField>

namespace LBDatabase {

class AttributeOwner;

class Table;
class Row;

class DatabaseAttribute : public Attribute
{
public:
    DatabaseAttribute(const QString &fieldName, Row *owner);

    QVariant data(int role = Qt::DisplayRole) const;

    void setValue(const QVariant &value);

    QString typeName() const;

    static QString sqlTypeName();

    void setData(const QVariant &value);

private:
    friend class Table;
    void initWithValue(const QVariant &value);

    Row *m_row;
    QString m_fieldName;
    QVariant m_value;
};

} // namespace LBDatabase

#endif // LBDATABASE_DATABASEATTRIBUTE_H
