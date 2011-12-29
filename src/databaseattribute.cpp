#include "databaseattribute.h"

#include "attributemetadata.h"
#include "database.h"
#include "row.h"
#include "table.h"

#include <QSqlDatabase>
#include <QVariant>
#include <QSqlRecord>
#include <QSqlQuery>

namespace LBDatabase {

DatabaseAttribute::DatabaseAttribute(const QString &fieldName, Row *owner) :
    Attribute(fieldName, owner),
    m_row(owner),
    m_fieldName(fieldName),
    m_value(QVariant())
{
    m_row->addDatabaseAttribute(this);
}

QVariant DatabaseAttribute::data(int) const
{
    return m_value;
}

void DatabaseAttribute::setData(const QVariant &value)
{
    setValue(value);
}

void DatabaseAttribute::initWithValue(const QVariant &value)
{
    m_value = value;
}

void DatabaseAttribute::setValue(const QVariant &value)
{
    QSqlDatabase database = m_row->table()->database()->sqlDatabase();
    QSqlQuery query(database);
    query.exec(QLatin1String("UPDATE ")+m_row->table()->name()+
                QLatin1String(" SET ")+m_fieldName+
                QLatin1String(" = '")+value.toString()+
               QLatin1String("' WHERE id = ")+QString::number(m_row->id())+
                QLatin1String(";"));
    query.finish();
    m_value = value;
}

QString DatabaseAttribute::typeName() const
{
    return m_value.typeName();
}

} // namespace LBDatabase
