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
const QString Attribute::NameColumn("name");
const QString Attribute::DisplayNameColumn("displayName");
const QString Attribute::EntityTypeIdColumn("entityTypeId");
const QString Attribute::PrefetchStrategyColumn("prefetchStrategy");

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

void Attribute::addPropertyValueToEntities()
{
    Q_D(Attribute);
    d->addPropertyValueToEntities();
}

void Attribute::addPropertyValue(Entity *entity)
{
    Q_D(Attribute);
    d->addPropertyValue(entity);
}

Attribute::~Attribute()
{
}

int Attribute::id() const
{
    Q_D(const Attribute);
    return d->row->id();
}

QString Attribute::name() const
{
    Q_D(const Attribute);
    return d->name;
}

QString Attribute::displayName(const Context *context) const
{
    Q_UNUSED(context)
    Q_D(const Attribute);
    return d->displayName;
}

Attribute::PrefetchStrategy Attribute::prefetchStrategy() const
{
    Q_D(const Attribute);
    return d->prefetchStrategy;
}

} // namespace LBDatabase
