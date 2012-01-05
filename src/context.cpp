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
namespace {
const QString NameColumn("name");
}

class ContextPrivate {
    ContextPrivate() : baseEntityType(0) {}

    void init();
    void initializeEntityHierarchy();
    void loadEntities();
    void initializeRelations();

    Row *row;
    Storage *storage;
    QString name;
    QHash<QString, EntityType *> entityTypes;
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
    name = row->data(NameColumn).toString();
}

void ContextPrivate::initializeEntityHierarchy()
{
    EntityType *parentType;
    foreach(EntityType *type, entityTypes) {
        parentType = storage->entityType(type->parentEntityTypeId());
        if(Q_LIKELY(parentType)) {
            type->setParentEntityType(parentType);
            parentType->addChildEntityType(type);
        }
        else {
            baseEntityType = type;
        }
    }

    baseEntityType->addPropertiesToChildren();
}

void ContextPrivate::loadEntities()
{
    Q_Q(Context);
    contextTable = storage->database()->table(name);
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

void ContextPrivate::initializeRelations()
{
    foreach(Entity *entity, entities) {
        entity->initializeRelations();
    }
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
    Q_D(Context);
    delete d;
}

QString Context::name() const
{
    Q_D(const Context);
    return d->name;
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

EntityType *Context::entityType(const QString &name) const
{
    Q_D(const Context);
    return d->entityTypes.value(name);
}

QList<EntityType *> Context::entityTypes() const
{
    Q_D(const Context);
    return d->entityTypes.values();
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

void Context::addEntityType(EntityType *type)
{
    Q_D(Context);
    if(d->entityTypes.contains(type->name()))
        return;

    d->entityTypes.insert(type->name(), type);
}

void Context::addAttribute(Attribute *attribute)
{
    Q_D(Context);
    if(d->properties.contains(attribute))
        return;

    d->properties.append(attribute);
}

void Context::addRelation(Relation *relation)
{
    Q_D(Context);
    if(d->properties.contains(relation))
        return;

    d->properties.append(relation);
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

void Context::initializeRelations()
{
    Q_D(Context);
    d->initializeRelations();
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

///*!
//  Implements QAbstractTableModel::setData()
//  */
//bool Context::setData(const QModelIndex &index, const QVariant &value, int role)
//{
//    if(role == Qt::EditRole) {
//        Q_D(const Table);
//        Row *row = d->rows.at(index.row());
//        row->setData(index.column(), value);
//        return true;
//    }
//    return false;
//}

///*!
//  Implements QAbstractTableModel::flags()
//  */
//Qt::ItemFlags Context::flags(const QModelIndex &index) const
//{
//    Q_D(const Table);
//    if(d->columns.at(index.column())->name() != QLatin1String("id")) {
//        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
//    }

//    return QAbstractItemModel::flags(index);
//}

} // namespace LBDatabase
