#include "row.h"

#include "attributemetadata.h"
#include "databaseattribute.h"
#include "table.h"
#include "tablemetadata.h"

#include <QSqlRecord>

#include <QVariant>
#include <QDebug>

namespace LBDatabase {

Row::Row() :
    QObject(),
    m_id(0),
    m_table(0)
{
}

Row::Row(const Row &other) :
    QObject(other.parent()),
    m_attributeByName(other.m_attributeByName),
    m_databaseAttributes(other.m_databaseAttributes),
    m_attributes(other.m_attributes),
    m_calculator(other.m_calculator)
{
}

Row::Row(int id, Table *parent) :
    QObject(parent),
    m_id(id),
    m_table(parent)
{
    initWith(id, parent);
}

void Row::initWith(int id, Table *parent)
{
    m_id = id;
    m_table = parent;

    foreach(AttributeMetaData *metaData, m_table->metaData()->attributeMetaDatas()) {
        metaData->createAttributeInstance(this);
    }
}

const int &Row::id() const
{
    return m_id;
}

Table *Row::table() const
{
    return m_table;
}

const QList<Attribute *> Row::attributes() const
{
    return m_attributes;
}

const QList<DatabaseAttribute*> Row::databaseAttributes() const
{
    return m_databaseAttributes;
}

void Row::addDatabaseAttribute(DatabaseAttribute *attribute)
{
    addAttribute(attribute);
    m_databaseAttributes.append(attribute);
}

Attribute *Row::attribute(const QString &name) const
{
    return m_attributeByName.value(name);
}

void Row::addAttribute(Attribute *attribute)
{
    m_attributeByName.insert(attribute->name(),attribute);
    m_attributes.append(attribute);
}

Calculator *Row::calculator() const
{
    return m_calculator;
}


} // namespace LBDatabase
