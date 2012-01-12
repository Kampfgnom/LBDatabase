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
/*!
  \class RelationValueLeft
  \brief The RelationValueLeft represents the left part of a relation.

  \ingroup highlevel-database-classes

  \todo Dokument
  */

/*!
  Creates a RelationValue.
  */
RelationValueLeft::RelationValueLeft(Relation *relation, Entity *parent) :
    RelationValue(*new RelationValueLeftPrivate, relation, parent)
{
}

/*!
  Destroys the relation value.
  */
RelationValueLeft::~RelationValueLeft()
{
}

/*!
  Does nothing.
  */
bool RelationValueLeft::setData(const QVariant &data)
{
    Q_UNUSED(data);
    return false;
}

/*!
  Returns a QVariant, which represents the content of the relation in the given role.

  Currently this method supports only the Qt::DisplayRole.
  */
QVariant RelationValueLeft::data(int role) const
{
    Q_UNUSED(role);
    Q_D(const RelationValueLeft);
    if(role == Qt::DisplayRole) {
        if(d->otherEntities.isEmpty())
            return QVariant();

        if(d->otherEntities.size() == 1)
            return d->otherEntities.at(0)->displayName();

        return QVariant::fromValue<QString>(QString::number(d->otherEntities.size())+QLatin1String(" ")+d->relation->entityTypeRight()->name()+QLatin1String("s"));
    }

    return QVariant();
}

/*!
  Does nothing. The left relation value does not fetch any content.
  */
void RelationValueLeft::fetchValue()
{
    Q_D(RelationValueLeft);
    d->fetchValue();
}

/*!
  \internal

  Registers the right relation value with this left value.
  */
void RelationValueLeft::setRightValue(RelationValueRight *right)
{
    Q_D(RelationValueLeft);
    d->rightValue = right;
    d->otherEntities.append(right->entities());
}

} // namespace LBDatabase
