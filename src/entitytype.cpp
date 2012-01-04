#include "entitytype.h"

#include "context.h"

namespace LBDatabase {

/******************************************************************************
** EntityTypePrivate
*/
class EntityTypePrivate {
    EntityTypePrivate() : context(0), parentEntityType(0) {}

    void init();

    QString name;
    Context *context;
    EntityType *parentEntityType;
    QString parentEntityTypeName;

    QList<EntityType *> childEntityTypes;

    EntityType * q_ptr;
    Q_DECLARE_PUBLIC(EntityType)
};

void EntityTypePrivate::init()
{
}

/******************************************************************************
** EntityType
*/
EntityType::EntityType(QObject *parent) :
    QObject(parent),
    d_ptr(new EntityTypePrivate)
{
    Q_D(EntityType);
    d->q_ptr = this;
    d->init();
}

EntityType::EntityType(Context *parent) :
    QObject(parent),
    d_ptr(new EntityTypePrivate)
{
    Q_D(EntityType);
    d->q_ptr = this;
    d->context = parent;
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

QString EntityType::parentEntityTypeName() const
{
    Q_D(const EntityType);
    return d->parentEntityTypeName;
}

QList<EntityType *> EntityType::childEntityTypes() const
{
    Q_D(const EntityType);
    return d->childEntityTypes;
}

void EntityType::setParentEntityTypeName(const QString &name)
{
    Q_D(EntityType);
    if(d->parentEntityTypeName == name)
        return;
    d->parentEntityTypeName = name;
}

} // namespace LBDatabase
