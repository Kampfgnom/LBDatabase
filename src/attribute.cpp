#include "attribute.h"

#include "attributevalue.h"
#include "context.h"
#include "entity.h"
#include "entitytype.h"
#include "row.h"
#include "storage.h"

namespace LBDatabase {

/******************************************************************************
** AttributePrivate
*/
class AttributePrivate {
    AttributePrivate() : prefetchStrategy(Attribute::PrefetchOnStartup) {}

    void init();
    void addPropertyValueToEntities();
    void addPropertyValue(Entity *entity);

    Row *row;
    Storage *storage;
    QString name;
    QString displayName;
    EntityType *entityType;

    Attribute::PrefetchStrategy prefetchStrategy;

    Attribute * q_ptr;
    Q_DECLARE_PUBLIC(Attribute)
};

void AttributePrivate::init()
{
    Q_Q(Attribute);
    name = row->data(Attribute::NameColumn).toString();
    displayName = row->data(Attribute::DisplayNameColumn).toString();
    prefetchStrategy = static_cast<Attribute::PrefetchStrategy>(row->data(Attribute::PrefetchStrategyColumn).toInt());

    entityType = storage->entityType(row->data(Attribute::EntityTypeIdColumn).toInt());
    entityType->addAttribute(q);
    entityType->context()->addAttribute(q);
}

void AttributePrivate::addPropertyValueToEntities()
{
    foreach(Entity *entity, entityType->entities()) {
        addPropertyValue(entity);
    }
}

void AttributePrivate::addPropertyValue(Entity *entity)
{
    Q_Q(Attribute);
    entity->addAttributeValue(new AttributeValue(q, entity));
}

/******************************************************************************
** Attribute
*/
/*!
  \class Attribtue
  \brief The Attribute class represents a simple single-value property of an
  EntityType.

  \ingroup highlevel-database-classes

  \todo Document when class is done.

  */

/*!
  \enum Attribute::PrefetchStrategy
  \brief Describes if and when the value of an attribute will be prefetched.

  The possible values are:

  \value PrefetchOnStartup The value will be cached, right when the storage is
  being opened.

  */

/*!
  The name of 'name' column.
  */
const QString Attribute::NameColumn("name");
/*!
  The name of 'displayName' column.
  */
const QString Attribute::DisplayNameColumn("displayName");
/*!
  The name of 'entityTypeId' column.
  */
const QString Attribute::EntityTypeIdColumn("entityTypeId");
/*!
  The name of 'prefetchStrategy' column.
  */
const QString Attribute::PrefetchStrategyColumn("prefetchStrategy");

/*!
  Creates an attribute, which contains the meta data from \a row in the given \a
  storage.
  */
Attribute::Attribute(Row *row, Storage *parent) :
    Property(parent),
    d_ptr(new AttributePrivate)
{
    Q_D(Attribute);
    d->q_ptr = this;
    d->row = row;
    d->storage = parent;
    d->init();
}

/*!
  Adds an AttributeValue instance to every existing Entity of the corrent
  EntityType.
  */
void Attribute::addPropertyValueToEntities()
{
    Q_D(Attribute);
    d->addPropertyValueToEntities();
}

/*!
  Adds an AttributeValue instance to the given \a entity.
  */
void Attribute::addPropertyValue(Entity *entity)
{
    Q_D(Attribute);
    d->addPropertyValue(entity);
}

/*!
  Destroys the attribute.
  */
Attribute::~Attribute()
{
}

/*!
  Returns the storage-global ID of the attribute.
  */
int Attribute::id() const
{
    Q_D(const Attribute);
    return d->row->id();
}

/*!
  Returns the name of the attribute. if the attribute is stored in the database
  this name is the name of the column where it is stored.
  */
QString Attribute::name() const
{
    Q_D(const Attribute);
    return d->name;
}

/*!
  Returns the display name of the attribute. Since each Attribute can only be
  part of one Context, the \a context parameter will be ignored.
  */
QString Attribute::displayName(const Context *context) const
{
    Q_UNUSED(context);
    Q_D(const Attribute);
    return d->displayName;
}

/*!
  Sets the display name of the attribute to \a displayName. Since each Attribute
  can only be part of one Context, the \a context parameter will be ignored.
  */
void Attribute::setDisplayName(const QString &displayName, const Context *context)
{
    Q_D(Attribute);
    Q_UNUSED(context);
    if(d->displayName == displayName)
        return;

    d->row->setData(Attribute::DisplayNameColumn, QVariant(displayName));
    d->displayName = displayName;
    emit displayNameChanged(displayName, d->entityType->context());
}

/*!
  Returns the prefetch strategy of the attribute. Currently every attribute is
  being prefetched when the storage is loaded.
  */
Attribute::PrefetchStrategy Attribute::prefetchStrategy() const
{
    Q_D(const Attribute);
    return d->prefetchStrategy;
}

} // namespace LBDatabase
