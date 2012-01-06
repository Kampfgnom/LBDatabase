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
namespace {
const QString NameColumn("name");
const QString DisplayNameColumn("displayName");
const QString EntityTypeIdColumn("entityTypeId");
const QString PrefetchStrategyColumn("prefetchStrategy");
const QString VisibilityColumn("visibility");
}

class AttributePrivate {
    AttributePrivate() : prefetchStrategy(Attribute::PrefetchOnStartup), visibility(Attribute::Visible) {}

    void init();
    void addPropertyValueToEntities();

    Row *row;
    Storage *storage;
    QString name;
    QString displayName;
    EntityType *entityType;

    Attribute::PrefetchStrategy prefetchStrategy;
    Attribute::Visibility visibility;

    Attribute * q_ptr;
    Q_DECLARE_PUBLIC(Attribute)
};

void AttributePrivate::init()
{
    Q_Q(Attribute);
    name = row->data(NameColumn).toString();
    displayName = row->data(DisplayNameColumn).toString();
    prefetchStrategy = static_cast<Attribute::PrefetchStrategy>(row->data(PrefetchStrategyColumn).toInt());
    visibility  = static_cast<Attribute::Visibility>(row->data(VisibilityColumn).toInt());

    entityType = storage->entityType(row->data(EntityTypeIdColumn).toInt());
    entityType->addAttribute(q);
    entityType->context()->addAttribute(q);
}

void AttributePrivate::addPropertyValueToEntities()
{
    Q_Q(Attribute);
    foreach(Entity *entity, entityType->entities()) {
        entity->addAttributeValue(new AttributeValue(q, entity));
    }
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

Attribute::Visibility Attribute::visibility() const
{
    Q_D(const Attribute);
    return d->visibility;
}

} // namespace LBDatabase
