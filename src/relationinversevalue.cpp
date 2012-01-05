#include "relationinversevalue.h"

#include "context.h"
#include "entity.h"
#include "entitytype.h"
#include "relation.h"
#include "relationvalue.h"
#include "row.h"
#include "storage.h"

#include <QDebug>

namespace LBDatabase {

/******************************************************************************
** RelationInverseValuePrivate
*/
class RelationInverseValuePrivate {
    RelationInverseValuePrivate() {}

    void init();
    void initRelationContent();
    void initOneToX();

    Entity *entity;
    Relation *relation;
    QList<Entity *> otherEntities;

    RelationInverseValue * q_ptr;
    Q_DECLARE_PUBLIC(RelationInverseValue)
};

void RelationInverseValuePrivate::init()
{
}

void RelationInverseValuePrivate::initRelationContent()
{
    switch(relation->cardinality()) {
    case Relation::OneToOne:
    case Relation::OneToMany:
        initOneToX();
        break;
    case Relation::ManyToMany:
        break;
    }
}

void RelationInverseValuePrivate::initOneToX()
{
    int otherId = entity->row()->data(relation->name()).toInt();
    Entity *otherEntity = relation->entityTypeLeft()->context()->entity(otherId);
    otherEntities.append(otherEntity);

    RelationValue *relationValue = qobject_cast<RelationValue *>(otherEntity->propertyValue(relation));
    if(relationValue) {
        relationValue->addEntityFromInverse(entity);
    }
}

/******************************************************************************
** RelationInverseValue
*/
RelationInverseValue::RelationInverseValue(Relation *relation, Entity *parent) :
    PropertyValue(parent),
    d_ptr(new RelationInverseValuePrivate)
{
    Q_D(RelationInverseValue);
    d->q_ptr = this;
    d->relation = relation;
    d->entity = parent;
    d->init();
}

RelationInverseValue::~RelationInverseValue()
{
    Q_D(RelationInverseValue);
    delete d;
}

QVariant RelationInverseValue::data(int role) const
{
    Q_UNUSED(role)
    Q_D(const RelationInverseValue);

    if(d->otherEntities.isEmpty())
        return QVariant();

    if(d->otherEntities.size() == 1)
        return d->otherEntities.at(0)->displayName();

    return QVariant::fromValue<QString>(QString::number(d->otherEntities.size())+QLatin1String(" ")+d->relation->entityTypeRight()->name()+QLatin1String("s"));
}

void RelationInverseValue::initRelationContent()
{
    Q_D(RelationInverseValue);
    d->initRelationContent();
}

Property *RelationInverseValue::property() const
{
    Q_D(const RelationInverseValue);
    return d->relation;
}

} // namespace LBDatabase
