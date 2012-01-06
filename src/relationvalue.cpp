#include "relationvalue.h"
#include "relationvalue_p.h"

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
}

void RelationValuePrivate::fetchValue()
{
}

void RelationValuePrivate::addOtherEntity(Entity *entity)
{
    if(!otherEntities.contains(entity))
        otherEntities.append(entity);
}

/******************************************************************************
** RelationValue
*/
RelationValue::RelationValue(Relation *relation, Entity *parent) :
    PropertyValue(parent),
    d_ptr(new RelationValuePrivate)
{
    Q_D(RelationValue);
    d->q_ptr = this;
    d->relation = relation;
    d->entity = parent;
    d->init();
}

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

QList<Entity *> RelationValue::entities() const
{
    Q_D(const RelationValue);
    return d->otherEntities;
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
