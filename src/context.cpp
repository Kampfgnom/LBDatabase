#include "context.h"

#include "entitytype.h"
#include "row.h"
#include "storage.h"

#include <QHash>

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

    Row *row;
    Storage *storage;
    QString name;
    QHash<QString, EntityType *> entityTypes;
    EntityType *baseEntityType;

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
        if(parentType) {
            type->setParentEntityType(parentType);
            parentType->addChildEntityType(type);
        }
        else {
            baseEntityType = type;
        }
    }
}

/******************************************************************************
** Context
*/
Context::Context(Row *row, Storage *parent) :
    QObject(parent),
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

void Context::addEntityType(EntityType *type)
{
    Q_D(Context);
    if(d->entityTypes.contains(type->name()))
        return;

    d->entityTypes.insert(type->name(), type);
}

void Context::initializeEntityHierarchy()
{
    Q_D(Context);
    d->initializeEntityHierarchy();
}

} // namespace LBDatabase
