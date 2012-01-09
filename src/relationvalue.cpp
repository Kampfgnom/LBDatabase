#include "relationvalue.h"
#include "relationvalue_p.h"

#include "context.h"
#include "entity.h"
#include "entitytype.h"
#include "relation.h"
#include "row.h"

namespace LBDatabase {

/******************************************************************************
** RelationValuePrivate
*/
void RelationValuePrivate::init()
{
    Q_Q(RelationValue);
    QObject::connect(q, SIGNAL(dataChanged(QVariant)), entity->context(), SLOT(onPropertyValueDataChanged(QVariant)));
}

void RelationValuePrivate::addOtherEntity(Entity *entity)
{
    if(!otherEntities.contains(entity))
        otherEntities.append(entity);
}

/******************************************************************************
** RelationValue
*/
RelationValue::RelationValue(RelationValuePrivate &dd, Relation *relation, Entity *parent) :
    PropertyValue(parent),
    d_ptr(&dd)
{
    Q_D(RelationValue);
    d->q_ptr = this;
    d->relation = relation;
    d->entity = parent;
    d->init();
}

RelationValue::~RelationValue()
{
}

Entity *RelationValue::entity() const
{
    Q_D(const RelationValue);
    return d->entity;
}

QList<Entity *> RelationValue::entities() const
{
    Q_D(const RelationValue);
    return d->otherEntities;
}

bool RelationValue::isEditable() const
{
    return false;
}

Property *RelationValue::property() const
{
    Q_D(const RelationValue);
    return d->relation;
}

void RelationValue::fetchValue()
{
    Q_D(RelationValue);
    d->fetchValue();
}

} // namespace LBDatabase
