#include "relationvalueright.h"
#include "relationvalue_p.h"

#include "context.h"
#include "entity.h"
#include "entitytype.h"
#include "relation.h"
#include "relationvalueleft.h"
#include "row.h"

#include <QDebug>

namespace LBDatabase {

/******************************************************************************
** RelationInverseValuePrivate
*/
class RelationValueRightPrivate : public RelationValuePrivate {
    RelationValueRightPrivate() {}

    void fetchValue();

    Q_DECLARE_PUBLIC(RelationValueRight)
};

void RelationValueRightPrivate::fetchValue()
{
    Q_Q(RelationValueRight);

    int leftId;
    Entity *leftEntity;
    RelationValueLeft *leftValue;

    switch(relation->cardinality()) {
    case Relation::OneToOne:
    case Relation::OneToMany:
        leftId = entity->row()->data(relation->name()).toInt();
        leftEntity = relation->entityTypeLeft()->context()->entity(leftId);
        otherEntities.append(leftEntity);
        leftValue = qobject_cast<RelationValueLeft *>(leftEntity->propertyValue(relation));
        leftValue->setRightValue(q);
        break;
    case Relation::ManyToMany:
        relation->initializeManyToManyRelation();
        break;
    }
}

/******************************************************************************
** RelationInverseValue
*/
RelationValueRight::RelationValueRight(Relation *relation, Entity *parent) :
    RelationValue(*new RelationValueRightPrivate, relation, parent)
{
}

RelationValueRight::~RelationValueRight()
{
}

bool RelationValueRight::setData(const QVariant &data)
{
    Q_UNUSED(data);
    return false;
}

QVariant RelationValueRight::data(int role) const
{
    Q_UNUSED(role);
    Q_D(const RelationValueRight);

    if(d->otherEntities.isEmpty())
        return QVariant();

    if(d->otherEntities.size() == 1)
        return d->otherEntities.at(0)->displayName();

    return QVariant::fromValue<QString>(QString::number(d->otherEntities.size())+QLatin1String(" ")+d->relation->entityTypeLeft()->name()+QLatin1String("s"));
}

void RelationValueRight::fetchValue()
{
    Q_D(RelationValueRight);
    d->fetchValue();
}

} // namespace LBDatabase
