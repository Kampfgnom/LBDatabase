#include "entitytype.h"

#include "context.h"
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
    void addInheritedAttributes(QList<Attribute *> attributes);

    Row *row;
    QString name;
    Context *context;
    Storage *storage;
    EntityType *parentEntityType;
    int parentEntityTypeId;

    QList<EntityType *> childEntityTypes;

    QList<Attribute *> attributes;
    QList<Attribute *> inheritedAttributes;
    QList<Attribute *> aggregatedAttributes;

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

void EntityTypePrivate::addInheritedAttributes(QList<Attribute *> attributes)
{
    inheritedAttributes.append(attributes);
    aggregatedAttributes.append(attributes);

    foreach(EntityType *type, childEntityTypes) {
        type->d_func()->addInheritedAttributes(aggregatedAttributes);
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

QList<Attribute *> EntityType::attributes() const
{
    Q_D(const EntityType);
    return d->attributes;
}

QList<Attribute *> EntityType::aggregatedAttributes() const
{
    Q_D(const EntityType);
    return d->aggregatedAttributes;
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
    d->attributes.append(attribute);
    d->aggregatedAttributes.append(attribute);
}

void EntityType::addAttributesToChildren()
{
    Q_D(EntityType);
    d->addInheritedAttributes(QList<Attribute *>());

}

} // namespace LBDatabase
