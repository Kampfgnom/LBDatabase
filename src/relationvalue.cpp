#include "relationvalue.h"

#include "entity.h"
#include "entitytype.h"
#include "relation.h"
#include "row.h"

namespace LBDatabase {

/******************************************************************************
** RelationValuePrivate
*/
class RelationValuePrivate {
    RelationValuePrivate() {}

    void init();
    void initOneToOne();
    void initRelationContent();

    Entity *entity;
    Relation *relation;
    QList<Entity *> otherEntities;

    RelationValue * q_ptr;
    Q_DECLARE_PUBLIC(RelationValue)
};

void RelationValuePrivate::init()
{
}

void RelationValuePrivate::initOneToOne()
{
}

void RelationValuePrivate::initRelationContent()
{
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

RelationValue::~RelationValue()
{
    Q_D(RelationValue);
    delete d;
}

QVariant RelationValue::data(int role) const
{
    Q_UNUSED(role)
    Q_D(const RelationValue);

    if(d->otherEntities.isEmpty())
        return QVariant();

    if(d->otherEntities.size() == 1)
        return d->otherEntities.at(0)->displayName();

    return QVariant::fromValue<QString>(QString::number(d->otherEntities.size())+QLatin1String(" ")+d->relation->entityTypeRight()->name()+QLatin1String("s"));
}

void RelationValue::addEntityFromInverse(Entity *entity)
{
    Q_D(RelationValue);
    d->otherEntities.append(entity);
}

void RelationValue::initRelationContent()
{
    Q_D(RelationValue);
    d->initRelationContent();
}

Property *RelationValue::property() const
{
    Q_D(const RelationValue);
    return d->relation;
}

} // namespace LBDatabase
