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
/*!
  \class Context
  \brief The Context class represents a context in a Storage.

  \ingroup highlevel-database-classes

  Each context is responsible for a hierarchy of EntityType and the concrete
  Entity instances of these types. It always has exactly one baseEntityType()
  from which other types in the context may be derived. The entityTypes() list
  contains every type in the context. You can add a new type with
  addEntityType().

  You may access the Entity instances of each context either by id (entity()),
  by index (entityAt()) or through the list of all entities(). You can insert
  new entities into the context by calling insertEntity().

  The Context class inherits QAbstractTableModel, so that you can add it as a
  model for any model-view based Qt class like QTreeView. This model is
  editable.

  */

/*!
  Creates a new context in the Storage \a parent, which is described by /a row.
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

/*!
  Destroys the context.
  */
Context::~Context()
{
}

/*!
  Returns the storage-global ID of the context.
  */
int Context::id() const
{
    Q_D(const Context);
    return d->row->id();
}

/*!
  Returns the name of the context. This name is also the name of the Sqlite
  table, which contains the Entity instances of the context.
  */
QString Context::name() const
{
    Q_D(const Context);
    return d->name;
}

//void Context::setName(const QString &name)
//{
//    Q_D(Context);
//    if(d->name == name)
//        return;

//    d->storage->database()->
//    d->row->setData(Context::NameColumn, QVariant(name));
//    d->name = name;
//    emit nameChanged(name);
//}

/*!
  Returns the storage, which contains the context.
  */
Storage *Context::storage() const
{
    Q_D(const Context);
    return d->storage;
}

/*!
  Returns the EntityType, from which all types in the context are somehow
  derived.
  */
EntityType *Context::baseEntityType() const
{
    Q_D(const Context);
    return d->baseEntityType;
}

/*!
  Returns a list of all types, for which the context is responsible.
  */
QList<EntityType *> Context::entityTypes() const
{
    Q_D(const Context);
    return d->entityTypes;
}

/*!
  Adds a new EntityType to the context, which will inherit all properties from
  \a parentEntityType.
  */
EntityType *Context::addEntityType(const QString &name, EntityType *parentEntityType)
{
    Q_D(Context);
    return d->addEntityType(name, parentEntityType);
}

/*!
  Returns the Entity instance with the ID \a id.
  */
Entity *Context::entity(int id) const
{
    Q_D(const Context);
    return d->entitiesById.value(id);
}

/*!
  Returns a list of all Entity instances.
  */
QList<Entity *> Context::entities() const
{
    Q_D(const Context);
    return d->entities;
}

/*!
  Inserts a new Entity of the type \a type into the context and returns the
  instance.
  */
Entity *Context::insertEntity(EntityType *type)
{
    Q_D(Context);
    return d->insertEntity(type);
}

/*!
  \internal

  Creates the base entity type for the context. This is only being used, when
  creating a new context.
  */
void Context::createBaseEntityType(const QString &name)
{
    Q_D(Context);
    d->createBaseEntityType(name);
}

/*!
  \internal

  Adds the EntityType \a type to the context. This is when loading the storage.
  */
void Context::addEntityType(EntityType *type)
{
    Q_D(Context);
    if(d->entityTypes.contains(type))
        return;

    connect(type, SIGNAL(nameChanged(QString)), this, SLOT(onEntityTypeNameChanged(QString)));
    d->entityTypes.append(type);
}

/*!
  \internal

  Adds the Attribute \a attribute to the context. This is when loading the storage.
  */
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

/*!
  \internal

  Adds the Relation \a relation to the context. This is when loading the storage.
  */
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

/*!
  \internal

  Initialized the hierarchy of the EntityTypes. This is when loading the storage.
  */
void Context::initializeEntityHierarchy()
{
    Q_D(Context);
    d->initializeEntityHierarchy();
}

/*!
  \internal

  Loads the Entity instances in this context. This is when loading the storage.
  */
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

/*!
  \internal
  Listens to name changes of EntityTypes and updates the model accordingly.
  */
void Context::onEntityTypeNameChanged(QString name)
{
    Q_D(const Context);
    Q_UNUSED(name);
    EntityType *type = static_cast<EntityType *>(sender());
    QModelIndex i = index(d->entityTypes.indexOf(type), 1);
    emit dataChanged(i, i);
}

/*!
  \internal
  Listens to name changes of Properties and updates the model accordingly.
  */
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

/*!
  \internal
  Listens to changes of PropertyValues and updates the model accordingly.
  */
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
