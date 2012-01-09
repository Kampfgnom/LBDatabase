#include "relationvalueleft.h"
#include "relationvalue_p.h"

#include "entity.h"
#include "entitytype.h"
#include "relation.h"
#include "relationvalueright.h"
#include "row.h"

namespace LBDatabase {

/******************************************************************************
** RelationInverseValuePrivate
*/
class RelationValueLeftPrivate : public RelationValuePrivate {
    RelationValueLeftPrivate() {}

    void fetchValue();

    RelationValueRight *rightValue;

    Q_DECLARE_PUBLIC(RelationValueLeft)
};

void RelationValueLeftPrivate::fetchValue()
{
}

/******************************************************************************
** RelationInverseValue
*/
RelationValueLeft::RelationValueLeft(Relation *relation, Entity *parent) :
    RelationValue(*new RelationValueLeftPrivate, relation, parent)
{
}

bool RelationValueLeft::setData(const QVariant &data)
{
    Q_UNUSED(data);
    return false;
}

RelationValueLeft::~RelationValueLeft()
{
}

QVariant RelationValueLeft::data(int role) const
{
    Q_UNUSED(role);
    Q_D(const RelationValueLeft);

    if(d->otherEntities.isEmpty())
        return QVariant();

    if(d->otherEntities.size() == 1)
        return d->otherEntities.at(0)->displayName();

    return QVariant::fromValue<QString>(QString::number(d->otherEntities.size())+QLatin1String(" ")+d->relation->entityTypeRight()->name()+QLatin1String("s"));
}

void RelationValueLeft::fetchValue()
{
    Q_D(RelationValueLeft);
    d->fetchValue();
}

void RelationValueLeft::setRightValue(RelationValueRight *right)
{
    Q_D(RelationValueLeft);
    d->rightValue = right;
    d->otherEntities.append(right->entities());
}

} // namespace LBDatabase
