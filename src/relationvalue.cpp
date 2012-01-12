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

//! \cond PRIVATE
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
//! \endcond

/******************************************************************************
** RelationValue
*/
/*!
  \class RelationValue
  \brief The RelationValue class represents a value of a relation between two
  EntityTypes.

  \ingroup highlevel-database-classes

  \todo Dokument
  */

/*!
  \var RelationValue::d_ptr
  \internal
  */

/*!
  Creates a RelationValue.
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

/*!
  Destroys the relation value.
  */
RelationValue::~RelationValue()
{
}

/*!
  Returns the entity of this relation value.
  */
Entity *RelationValue::entity() const
{
    Q_D(const RelationValue);
    return d->entity;
}

/*!
  Returns the entities to which this relation maps.
  */
QList<Entity *> RelationValue::entities() const
{
    Q_D(const RelationValue);
    return d->otherEntities;
}

/*!
  Returns false
  */
bool RelationValue::isEditable() const
{
    return false;
}

/*!
  Returns the relation, that describes this value.
  */
Property *RelationValue::property() const
{
    Q_D(const RelationValue);
    return d->relation;
}

/*!
  Initializes the content of the relation value.
  */
void RelationValue::fetchValue()
{
    Q_D(RelationValue);
    d->fetchValue();
}

} // namespace LBDatabase
