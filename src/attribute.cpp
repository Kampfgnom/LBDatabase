#include "attribute.h"

#include "context.h"
#include "entitytype.h"
#include "row.h"
#include "storage.h"

namespace LBDatabase {

/******************************************************************************
** AttributePrivate
*/
namespace {
const QString NameColumn("name");
const QString DisplayNameColumn("displayName");
const QString EntityTypeIdColumn("entityTypeId");
const QString PrefetchStrategyColumn("prefetchStrategy");
}

class AttributePrivate {
    AttributePrivate() : prefetchStrategy(Attribute::NoPrefretch) {}

    void init();

    Row *row;
    Storage *storage;
    QString name;
    QString displayName;
    EntityType *type;

    Attribute::PrefetchStrategy prefetchStrategy;

    Attribute * q_ptr;
    Q_DECLARE_PUBLIC(Attribute)
};

void AttributePrivate::init()
{
    Q_Q(Attribute);
    name = row->data(NameColumn).toString();
    displayName = row->data(DisplayNameColumn).toString();
    prefetchStrategy = static_cast<Attribute::PrefetchStrategy>(row->data(PrefetchStrategyColumn).toInt());

    type = storage->entityType(row->data(EntityTypeIdColumn).toInt());
    type->addAttribute(q);
    type->context()->addAttribute(q);
}

/******************************************************************************
** Attribute
*/
Attribute::Attribute(Row *row, Storage *parent) :
    QObject(parent),
    d_ptr(new AttributePrivate)
{
    Q_D(Attribute);
    d->q_ptr = this;
    d->row = row;
    d->storage = parent;
    d->init();
}

Attribute::~Attribute()
{
    Q_D(Attribute);
    delete d;
}

void Attribute::setName(const QString &name)
{
    Q_D(Attribute);
    if(d->name == name)
        return;
    d->name = name;
    emit nameChanged(name);
}

QString Attribute::name() const
{
    Q_D(const Attribute);
    return d->name;
}

QString Attribute::displayName() const
{
    Q_D(const Attribute);
    return d->displayName;
}

Attribute::PrefetchStrategy Attribute::prefetchStrategy() const
{
    Q_D(const Attribute);
    return d->prefetchStrategy;
}

} // namespace LBDatabase
