#include "relation.h"

#include "database.h"
#include "row.h"
#include "table.h"

#include <QVariant>

namespace LBDatabase {

//Relation::Relation(const QString &displayName, const QString &name, const QString &displayingAttribute, Table *table, Row *owner) :
//    Attribute(name, displayName, owner),
//    m_table(table),
//    m_displayingAttribute(displayingAttribute)
//{
//    m_relationId = new DatabaseAttribute(name+QLatin1String(" ID"), table, name+QLatin1String("Id"), owner);
//    m_value = QVariant::fromValue<Row *>(table->rowById(m_relationId->data().toInt()));
//}

//QVariant Relation::data(int role) const
//{
//    if(role == Qt::DisplayRole) {

//    }

//    return QVariant();
//}

} // namespace LBDatabase
