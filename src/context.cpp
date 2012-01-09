#include "context.h"

#include "attribute.h"
#include "attributevalue.h"
#include "database.h"
#include "entity.h"
#include "entitytype.h"
#include "relation.h"
#include "row.h"
#include "storage.h"
#include "table.h"

#include <QHash>
#include <QList>
#include <QDebug>

namespace LBDatabase {

/******************************************************************************
** ContextPrivate
*/
const QString Context::NameColumn("name");

class ContextPrivate {
    ContextPrivate() : baseEntityType(0) {}

    void init();
    void initializeEntityHierarchy();
    void loadEntities();
    void initializeRelations();
    void fillRelations();
    void createBaseEntityType(const QString &name);

    EntityType *addEntityType(const QString &name, EntityType *parentEntityType);
    Entity *insertEntity(EntityType *type);


    Row *row;
    Storage *storage;
    QString name;
    QList<EntityType *> entityTypes;
    EntityType *baseEntityType;
    Table *contextTable;
    QList<Entity *> entities;
    QHash<int, Entity *> entitiesById;
    QList<Property *> properties;

    Context * q_ptr;
    Q_DECLARE_PUBLIC(Context)
};

void ContextPrivate::init()
{
    name = row->data(Context::NameColumn).toString();
    contextTable = storage->database()->table(name);
}

void ContextPrivate::initializeEntityHierarchy()
{
    EntityType *parentType;
    foreach(EntityType *type, entityTypes) {
        parentType = storage->entityType(type->parentEntityTypeId());
        if(parentType) {
            type->setParentEntityType(parentType);
            parentType->addChildEntityType(type);
        }
        else {
            baseEntityType = type;
        }
    }
    foreach(EntityType *child, baseEntityType->childEntityTypes()) {
        child->addInheritedProperties(baseEntityType);
    }
}

void ContextPrivate::loadEntities()
{
    Q_Q(Context);
    if(!contextTable)
        return;

    entities.reserve(contextTable->rows().size());
    entitiesById.reserve(contextTable->rows().size());
    foreach(Row *row, contextTable->rows()) {
        Entity *entity = new Entity(row, q);
        entities.append(entity);
        entitiesById.insert(row->id(), entity);
    }
}

EntityType *ContextPrivate::addEntityType(const QString &name, EntityType *parentEntityType)
{
    Row *entityTypeRow = storage->entitiesTable()->appendRow();
    entityTypeRow->setData(EntityType::NameColumn, QVariant(name));
    entityTypeRow->setData(EntityType::ParentEntityTypeIdColumn, QVariant(parentEntityType->id()));
    entityTypeRow->setData(EntityType::ContextColumn, QVariant(row->id()));

    EntityType *type = new EntityType(row, storage);
    type->setParentEntityType(parentEntityType);
    parentEntityType->addChildEntityType(type);
    storage->insertEntityType(type);

    type->addInheritedProperties(parentEntityType);
    return type;
}

Entity *ContextPrivate::insertEntity(EntityType *type)
{
    Q_Q(Context);
    Row *row = contextTable->appendRow();
    row->setData(Entity::EntityTypeIdColumn, QVariant(type->id()));

    q->beginInsertRows(QModelIndex(), entities.size(), entities.size());
    Entity *entity = new Entity(row, q);
    entities.append(entity);
    entitiesById.insert(row->id(), entity);

    foreach(Property *property, type->properties()) {
        property->addPropertyValue(entity);
    }
    foreach(PropertyValue *value, entity->propertyValues()) {
        value->fetchValue();
    }
    q->endInsertRows();

    return entity;
}

void ContextPrivate::createBaseEntityType(const QString &name)
{
    Row *entityTypeRow = storage->entitiesTable()->appendRow();
    entityTypeRow->setData(EntityType::NameColumn, QVariant(name));
    entityTypeRow->setData(EntityType::ParentEntityTypeIdColumn, QVariant());
    entityTypeRow->setData(EntityType::ContextColumn, QVariant(row->id()));

    baseEntityType = new EntityType(row, storage);
    storage->insertEntityType(baseEntityType);
}

/******************************************************************************
** Context
*/
Context::Context(Row *row, Storage *parent) :
    QAbstractTableModel(parent),
    d_ptr(new ContextPrivate)
{
    Q_D(Context);
    d->q_ptr = this;
    d->row = row;
    d->storage = parent;
    d->init();
}

Context::~Context()
{
}

int Context::id() const
{
    Q_D(const Context);
    return d->row->id();
}

QString Context::name() const
{
    Q_D(const Context);
    return d->name;
}

void Context::setName(const QString &name)
{
    Q_D(Context);
    if(d->name == name)
        return;

    d->row->setData(Context::NameColumn, QVariant(name));
    d->name = name;
    emit nameChanged(name);
}

Storage *Context::storage() const
{
    Q_D(const Context);
    return d->storage;
}

EntityType *Context::baseEntityType() const
{
    Q_D(const Context);
    return d->baseEntityType;
}

QList<EntityType *> Context::entityTypes() const
{
    Q_D(const Context);
    return d->entityTypes;
}

EntityType *Context::addEntityType(const QString &name, EntityType *parentIntityType)
{
    Q_D(Context);
    return d->addEntityType(name, parentIntityType);
}

Entity *Context::entity(int id) const
{
    Q_D(const Context);
    return d->entitiesById.value(id);
}

Entity *Context::entityAt(int index) const
{
    Q_D(const Context);
    return d->entities.at(index);
}

QList<Entity *> Context::entities() const
{
    Q_D(const Context);
    return d->entities;
}

Entity *Context::insertEntity(EntityType *type)
{
    Q_D(Context);
    return d->insertEntity(type);
}

void Context::createBaseEntityType(const QString &name)
{
    Q_D(Context);
    d->createBaseEntityType(name);
}

void Context::addEntityType(EntityType *type)
{
    Q_D(Context);
    if(d->entityTypes.contains(type))
        return;

    connect(type, SIGNAL(nameChanged(QString)), this, SLOT(onEntityTypeNameChanged(QString)));
    d->entityTypes.append(type);
}

void Context::addAttribute(Attribute *attribute)
{
    Q_D(Context);
    if(d->properties.contains(attribute))
        return;

    beginInsertColumns(QModelIndex(), d->properties.size(), d->properties.size());
    d->properties.append(attribute);
    connect(attribute, SIGNAL(displayNameChanged(QString,Context*)), this, SLOT(onPropertyDisplayNameChanged(QString,Context*)));
    endInsertColumns();
}

void Context::addRelation(Relation *relation)
{
    Q_D(Context);
    if(d->properties.contains(relation))
        return;

    beginInsertColumns(QModelIndex(), d->properties.size(), d->properties.size());
    d->properties.append(relation);
    connect(relation, SIGNAL(displayNameChanged(QString,Context*)), this, SLOT(onPropertyDisplayNameChanged(QString,Context*)));
    endInsertColumns();
}

void Context::initializeEntityHierarchy()
{
    Q_D(Context);
    d->initializeEntityHierarchy();
}

void Context::loadEntities()
{
    Q_D(Context);
    d->loadEntities();
}

/*!
  Implements QAbstractTableModel::data()
  */
QVariant Context::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DisplayRole || role == Qt::EditRole) {
        Q_D(const Context);
        Entity *entity = d->entities.at(index.row());
        switch(index.column()) {
        case 0:
            return entity->row()->id();
        case 1:
            return entity->entityType()->name();
        default:
            return entity->data(d->properties.at(index.column() - 2));
        }
    }

    return QVariant();
}

/*!
  Implements QAbstractTableModel::headerData()
  */
QVariant Context::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal) {
        if(role == Qt::DisplayRole) {
            Q_D(const Context);
            switch(section) {
            case 0:
                return QLatin1String("ID");
            case 1:
                return QLatin1String("Type");
            default:
                return d->properties.at(section - 2)->displayName(this);
            }
        }
        else if(role == Qt::TextAlignmentRole) {
            return Qt::AlignLeft;
        }
    }
    return QVariant();
}

/*!
  Implements QAbstractTableModel::columnCount()
  */
int Context::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid()) {
        return 0;
    }
    Q_D(const Context);
    return d->properties.size() + 2;
}

/*!
  Implements QAbstractTableModel::rowCount()
  */
int Context::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid()) {
        return 0;
    }
    Q_D(const Context);
    return d->entities.size();
}

void Context::onEntityTypeNameChanged(QString name)
{
    Q_D(const Context);
    Q_UNUSED(name);
    EntityType *type = static_cast<EntityType *>(sender());
    QModelIndex i = index(d->entityTypes.indexOf(type), 1);
    emit dataChanged(i, i);
}

void Context::onPropertyDisplayNameChanged(QString displayName, Context *context)
{
    Q_D(const Context);
    Q_UNUSED(displayName);
    if(context == this) {
        Property *p = static_cast<Property *>(sender());
        int i =d->properties.indexOf(p);
        emit headerDataChanged(Qt::Horizontal, i, i);
    }
}

void Context::onPropertyValueDataChanged(QVariant data)
{
    Q_D(const Context);
    Q_UNUSED(data);

    PropertyValue *v = static_cast<PropertyValue *>(sender());
    QModelIndex i = index(d->entities.indexOf(v->entity()), d->properties.indexOf(v->property()));
    emit dataChanged(i, i);
}

/*!
  Implements QAbstractTableModel::setData()
  */
bool Context::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role == Qt::EditRole) {
        Q_D(const Context);
        Entity *e = d->entities.at(index.row());
        PropertyValue *v = e->propertyValue(d->properties.value(index.column() - 2));
        if(v)
            return v->setData(value);
    }
    return false;
}

/*!
  Implements QAbstractTableModel::flags()
  */
Qt::ItemFlags Context::flags(const QModelIndex &index) const
{
    Q_D(const Context);
    if(index.column() > 1) {
        Entity *e = d->entities.at(index.row());
        PropertyValue *v = e->propertyValue(d->properties.value(index.column() - 2));
        if(v && v->isEditable()) {
            return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
        }
    }

    return QAbstractItemModel::flags(index);
}

} // namespace LBDatabase
