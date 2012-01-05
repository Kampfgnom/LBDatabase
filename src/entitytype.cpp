#include "entitytype.h"

#include "attribute.h"
#include "context.h"
#include "property.h"
#include "relation.h"
#include "row.h"
#include "storage.h"

namespace LBDatabase {

/******************************************************************************
** EntityTypePrivate
*/
namespace {
const QString ContextColumn("contextId");
const QString NameColumn("name");
const QString ParentEntityTypeIdColumn("parentEntityTypeId");
}

class EntityTypePrivate {
    EntityTypePrivate() : context(0), parentEntityType(0) {}

    void init();
    void addInheritedProperties(QList<Attribute *> attributes, QList<Relation *> newRelations);

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

    EntityType * q_ptr;
    Q_DECLARE_PUBLIC(EntityType)
};

void EntityTypePrivate::init()
{
    Q_Q(EntityType);
    name = row->data(NameColumn).toString();
    parentEntityTypeId = row->data(ParentEntityTypeIdColumn).toInt();
    int contextId = row->data(ContextColumn).toInt();
    context = storage->context(contextId);
    context->addEntityType(q);
}

void EntityTypePrivate::addInheritedProperties(QList<Attribute *> newAttributes, QList<Relation *> newRelations)
{
    properties.reserve(attributes.size() + newRelations.size());
    attributes.reserve(attributes.size());
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
        type->d_func()->addInheritedProperties(attributes, relations);
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
    Q_D(EntityType);
    delete d;
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

void EntityType::addPropertiesToChildren()
{
    Q_D(EntityType);
    d->addInheritedProperties(QList<Attribute *>(), QList<Relation *>());
}

} // namespace LBDatabase