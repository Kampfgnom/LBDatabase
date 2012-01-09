#include "entitytype.h"

#include "attribute.h"
#include "column.h"
#include "context.h"
#include "database.h"
#include "entity.h"
#include "property.h"
#include "propertyvalue.h"
#include "relation.h"
#include "row.h"
#include "storage.h"
#include "table.h"

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

    static QString typeToSql(EntityType::Type type);

    void init();
    void addInheritedProperties(EntityType *parent);
    Attribute *addAttribute(const QString &name, EntityType::Type type);
    Relation *addRelation(const QString &name, EntityType *otherType, Relation::Cardinality cardinality);

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

QString EntityTypePrivate::typeToSql(EntityType::Type type)
{
    switch(type) {
    case EntityType::Text:
        return Column::typeToName(Column::Text);
    case EntityType::Integer:
        return Column::typeToName(Column::Integer);
    case EntityType::Real:
        return Column::typeToName(Column::Real);
    case EntityType::Unkown:
    default:
        return Column::typeToName(Column::Blob);
    }
}

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

Attribute *EntityTypePrivate::addAttribute(const QString &name, EntityType::Type type)
{
    Table *contextTable = storage->database()->table(context->name());
    contextTable->addColumn(name, EntityTypePrivate::typeToSql(type));

    Table *entitiesTable = storage->attributesTable();
    Row *row = entitiesTable->appendRow();
    row->setData(Attribute::NameColumn, QVariant(name));
    row->setData(Attribute::DisplayNameColumn, QVariant(name));
    row->setData(Attribute::EntityTypeIdColumn, QVariant(row->id()));
    row->setData(Attribute::PrefetchStrategyColumn, QVariant(static_cast<int>(Attribute::PrefetchOnStartup)));

    Attribute *attribute = new Attribute(row, storage);
    storage->insertAttribute(attribute);
    attribute->addPropertyValueToEntities();
    foreach(Entity *entity, entities) {
        entity->propertyValue(attribute)->fetchValue();
    }

    return attribute;
}

Relation *EntityTypePrivate::addRelation(const QString &name, EntityType *otherType, Relation::Cardinality cardinality)
{
    Q_Q(EntityType);
    Row *row = storage->contextsTable()->appendRow();
    row->setData(Relation::NameColumn, QVariant(name));
    row->setData(Relation::EntityTypeLeftColumn, QVariant(q->id()));
    row->setData(Relation::EntityTypeRightColumn, QVariant(otherType->id()));
    row->setData(Relation::DisplayNameLeftColumn, QVariant(name));
    row->setData(Relation::DisplayNameRightColumn, QVariant(name));
    row->setData(Relation::CardinalityColumn, QVariant(static_cast<int>(cardinality)));

    if(cardinality == Relation::OneToMany || cardinality == Relation::OneToOne) {
        storage->database()->table(otherType->name())->addColumn(name, Column::typeToName(Column::Integer));
    }
    else if(cardinality == Relation::ManyToMany) {
        Table *table = storage->database()->createTable(name);
        table->addColumn(q->name(), Column::typeToName(Column::Integer));
        table->addColumn(otherType->name(), Column::typeToName(Column::Integer));
    }

    Relation *relation = new Relation(row, storage);
    storage->insertRelation(relation);
    relation->addPropertyValueToEntities();
    foreach(Entity *entity, entities) {
        entity->propertyValue(relation)->fetchValue();
    }

    return relation;
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

    d->row->setData(EntityType::NameColumn, QVariant(name));
    d->name = name;
    emit nameChanged(name);
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

Attribute *EntityType::addAttribute(const QString &name, Type type)
{
    Q_D(EntityType);
    return d->addAttribute(name, type);
}

Relation *EntityType::addRelation(const QString &name, EntityType *otherType, Relation::Cardinality cardinality)
{
    Q_D(EntityType);
    return d->addRelation(name, otherType, cardinality);
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
