#include "entitytype.h"

#include "attribute.h"
#include "context.h"
#include "property.h"
#include "relation.h"
#include "row.h"
#include "storage.h"

#include <QDebug>

namespace LBDatabase {

/******************************************************************************
** EntityTypePrivate
*/
const QString EntityType::ContextColumn("contextId");
const QString EntityType::NameColumn("name");
const QString EntityType::ParentEntityTypeIdColumn("parentEntityTypeId");

class EntityTypePrivate {
    EntityTypePrivate() : context(0), parentEntityType(0) {}

    void init();
    void addInheritedProperties(EntityType *parent);

    Row *row;
    QString name;
    Context *context;
    Storage *storage;
    EntityType *parentEntityType;
    int parentEntityTypeId;

    QList<EntityType *> childEntityTypes;

    QList<Property *> properties;
    QList<Attribute *> attributes;
    QList<Relation *> relations;
    QList<Entity *> entities;

    EntityType * q_ptr;
    Q_DECLARE_PUBLIC(EntityType)
};

void EntityTypePrivate::init()
{
    Q_Q(EntityType);
    name = row->data(EntityType::NameColumn).toString();
    parentEntityTypeId = row->data(EntityType::ParentEntityTypeIdColumn).toInt();
    int contextId = row->data(EntityType::ContextColumn).toInt();
    context = storage->context(contextId);
    context->addEntityType(q);
}

void EntityTypePrivate::addInheritedProperties(EntityType *parent)
{
    Q_Q(EntityType);

    QList<Relation *> newRelations = parent->relations();
    QList<Attribute *> newAttributes = parent->attributes();

    properties.reserve(newAttributes.size() + newRelations.size());
    attributes.reserve(newAttributes.size());
    relations.reserve(newRelations.size());

    foreach(Attribute *attribute, newAttributes) {
        properties.append(attribute);
    }
    foreach(Relation *relation, newRelations) {
        properties.append(relation);
    }

    relations.append(newRelations);
    attributes.append(newAttributes);

    foreach(EntityType *type, childEntityTypes) {
        type->d_func()->addInheritedProperties(q);
    }
}

/******************************************************************************
** EntityType
*/
EntityType::EntityType(Row *row, Storage *parent) :
    QObject(parent),
    d_ptr(new EntityTypePrivate)
{
    Q_D(EntityType);
    d->q_ptr = this;
    d->row = row;
    d->storage = parent;
    d->init();
}

EntityType::~EntityType()
{
}

int EntityType::id() const
{
    Q_D(const EntityType);
    return d->row->id();
}

QString EntityType::name() const
{
    Q_D(const EntityType);
    return d->name;
}

void EntityType::setName(const QString &name)
{
    Q_D(EntityType);
    if(d->name == name)
        return;
    d->name = name;
}

LBDatabase::Context *EntityType::context() const
{
    Q_D(const EntityType);
    return d->context;
}

EntityType *EntityType::parentEntityType() const
{
    Q_D(const EntityType);
    return d->parentEntityType;
}

void EntityType::setContext(LBDatabase::Context *context)
{
    Q_D(EntityType);
    if(d->context == context)
        return;
    d->context = context;
}

void EntityType::addChildEntityType(EntityType *type)
{
    Q_D(EntityType);
    if(d->childEntityTypes.contains(type))
        return;

    d->childEntityTypes.append(type);
}

void EntityType::setParentEntityType(EntityType *type)
{
    Q_D(EntityType);
    if(d->parentEntityType == type)
        return;
    d->parentEntityType = type;
}

int EntityType::parentEntityTypeId() const
{
    Q_D(const EntityType);
    return d->parentEntityTypeId;
}

QList<EntityType *> EntityType::childEntityTypes() const
{
    Q_D(const EntityType);
    return d->childEntityTypes;
}

QList<Property *> EntityType::properties() const
{
    Q_D(const EntityType);
    return d->properties;
}

QList<Attribute *> EntityType::attributes() const
{
    Q_D(const EntityType);
    return d->attributes;
}

QList<Relation *> EntityType::relations() const
{
    Q_D(const EntityType);
    return d->relations;
}

Attribute *EntityType::addAttribute(const QString &name)
{
    qWarning() << "EntityType::addAttribute: IMPLEMENT ME";
}

QList<Entity *> EntityType::entities() const
{
    Q_D(const EntityType);
    return d->entities;
}

bool EntityType::inherits(EntityType *entityType) const
{
    Q_D(const EntityType);

    if(d->parentEntityType == entityType || this == entityType)
        return true;

    if(!d->parentEntityType || !entityType)
        return false;

    return d->parentEntityType->inherits(entityType);
}

void EntityType::setParentEntityTypeId(int id)
{
    Q_D(EntityType);
    if(d->parentEntityTypeId == id)
        return;
    d->parentEntityTypeId = id;
}

void EntityType::addAttribute(Attribute *attribute)
{
    Q_D(EntityType);
    d->properties.append(attribute);
    d->attributes.append(attribute);
}

void EntityType::addRelation(Relation *relation)
{
    Q_D(EntityType);
    d->properties.append(relation);
    d->relations.append(relation);
}

void EntityType::addInheritedProperties(EntityType *parent)
{
    Q_D(EntityType);
    d->addInheritedProperties(parent);
}

void EntityType::addEntity(Entity *entity)
{
    Q_D(EntityType);
    d->entities.append(entity);
    if(d->parentEntityType)
        d->parentEntityType->addEntity(entity);
}

} // namespace LBDatabase
