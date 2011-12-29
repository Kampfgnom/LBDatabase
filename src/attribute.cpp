#include "attribute.h"

#include "attributemetadata.h"
#include "row.h"
#include "table.h"
#include "tablemetadata.h"

#include <QVariant>

#include <QDebug>

namespace LBDatabase {

Attribute::Attribute(const QString &name, Row *owner) :
    QObject(owner),
    m_owner(owner),
    m_metaData(owner->table()->metaData()->attributeMetaData(name))
{
}

QString Attribute::name() const
{
    return m_metaData->name();
}

QString Attribute::displayName() const
{
    return m_metaData->displayName();
}

Row *Attribute::owner() const
{
    return m_owner;
}

bool Attribute::isEditable() const
{
    return m_metaData->isEditable();
}

AttributeMetaData *Attribute::metaData() const
{
    return m_metaData;
}

} // namespace LBDatabase
