#include "context.h"

#include "entitytype.h"

#include <QHash>

namespace LBDatabase {

/******************************************************************************
** ContextPrivate
*/
class ContextPrivate {
    ContextPrivate() : baseEntityType(0) {}

    void init();

    QString name;
    QHash<QString, EntityType *> entityTypes;
    EntityType *baseEntityType;

    Context * q_ptr;
    Q_DECLARE_PUBLIC(Context)
};

void ContextPrivate::init()
{
    foreach(EntityType *type, entityTypes) {
        QString parent = type->parentEntityTypeName();
        if(!parent.isEmpty()) {
            EntityType *parentType = entityTypes.value(parent);
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
Context::Context(const QString &name, QObject *parent) :
    QObject(parent),
    d_ptr(new ContextPrivate)
{
    Q_D(Context);
    d->q_ptr = this;
    d->name = name;
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

void Context::init()
{
    Q_D(Context);
    d->init();
}

} // namespace LBDatabase
