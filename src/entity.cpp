#include "entity.h"

#include "attribute.h"
#include "attributevalue.h"
#include "context.h"
#include "entitytype.h"
#include "relation.h"
#include "relationvalueright.h"
#include "relationvalue.h"
#include "row.h"
#include "storage.h"

#include <QDebug>

namespace LBDatabase {

/******************************************************************************
** EntityPrivate
*/
const QString Entity::EntityTypeIdColumn("entityTypeId");

class EntityPrivate {
    EntityPrivate() {}

    void init();
    void initializeRelations();
    void initializeRelationContent();

    Storage *storage;
    Context *context;
    Row *row;
    EntityType *entityType;
    QHash<Property *, PropertyValue *> propertyValues;

    QList<AttributeValue *> attributeValues;
    QList<RelationValue *> relationValues;

    Entity * q_ptr;
    Q_DECLARE_PUBLIC(Entity)
};

void EntityPrivate::init()
{
    Q_Q(Entity);
    storage = context->storage();
    entityType = storage->entityType(row->data(Entity::EntityTypeIdColumn).toInt());
    entityType->addEntity(q);
}

/******************************************************************************
** Entity
*/
Entity::Entity(Row *row, Context *parent) :
    QObject(parent),
    d_ptr(new EntityPrivate)
{
    Q_D(Entity);
    d->q_ptr = this;
    d->context = parent;
    d->row = row;
    d->init();
}

Entity::~Entity()
{
}

QString Entity::displayName(int role) const
{
    Q_UNUSED(role);
    Q_D(const Entity);
    return d->entityType->name()+QLatin1String(" ID: ")+QString::number(d->row->id());
}

QVariant Entity::data(Property *property) const
{
    Q_D(const Entity);
    PropertyValue *propertyValue = d->propertyValues.value(property, 0);
    if(!propertyValue)
        return QVariant();

    return propertyValue->data();
}

bool Entity::setData(const QVariant &data, Property *property)
{
    Q_D(const Entity);

    PropertyValue *propertyValue = d->propertyValues.value(property, 0);
    if(!propertyValue)
        return false;

    return propertyValue->setData(data);
}

EntityType *Entity::entityType() const
{
    Q_D(const Entity);
    return d->entityType;
}

Storage *Entity::storage() const
{
    Q_D(const Entity);
    return d->storage;
}

Context *Entity::context() const
{
    Q_D(const Entity);
    return d->context;
}

QList<PropertyValue *> Entity::propertyValues() const
{
    Q_D(const Entity);
    return d->propertyValues.values();
}

PropertyValue *Entity::propertyValue(Property *property) const
{
    Q_D(const Entity);
    return d->propertyValues.value(property, 0);
}

Row *Entity::row() const
{
    Q_D(const Entity);
    return d->row;
}

void Entity::addAttributeValue(AttributeValue *value)
{
    Q_D(Entity);
    d->propertyValues.insert(value->property(), value);
    d->attributeValues.append(value);
}

void Entity::addRelationValue(RelationValue *value)
{
    Q_D(Entity);
    d->propertyValues.insert(value->property(), value);
    d->relationValues.append(value);
}

} // namespace LBDatabase
