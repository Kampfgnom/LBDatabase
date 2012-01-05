#include "entity.h"

#include "attribute.h"
#include "attributevalue.h"
#include "entitytype.h"
#include "row.h"
#include "storage.h"

namespace LBDatabase {

/******************************************************************************
** EntityPrivate
*/
namespace {
const QString EntityTypeIdColumn("entityTypeId");
}

class EntityPrivate {
    EntityPrivate() {}

    void init();

    Storage *storage;
    Row *row;
    EntityType *type;
    QHash<Attribute *, AttributeValue *> attributeValues;

    Entity * q_ptr;
    Q_DECLARE_PUBLIC(Entity)
};

void EntityPrivate::init()
{
    Q_Q(Entity);
    type = storage->entityType(row->data(EntityTypeIdColumn).toInt());

    foreach(Attribute *attribute, type->aggregatedAttributes()) {
        AttributeValue *value = new AttributeValue(attribute, q);
        attributeValues.insert(attribute, value);
    }
}

/******************************************************************************
** Entity
*/
Entity::Entity(Row *row, Storage *parent) :
    QObject(parent),
    d_ptr(new EntityPrivate)
{
    Q_D(Entity);
    d->q_ptr = this;
    d->storage = parent;
    d->row = row;
    d->init();
}

Entity::~Entity()
{
    Q_D(Entity);
    delete d;
}

QVariant Entity::value(Attribute *attribute) const
{
    Q_D(const Entity);
    AttributeValue *value = d->attributeValues.value(attribute, 0);

    if(!value)
        return QVariant();

    return value->value();
}

EntityType *Entity::entityType() const
{
    Q_D(const Entity);
    return d->type;
}

QVariant Entity::data(const QString &attributeName) const
{
    Q_D(const Entity);
    return d->row->data(attributeName);
}

Row *Entity::row() const
{
    Q_D(const Entity);
    return d->row;
}

} // namespace LBDatabase
