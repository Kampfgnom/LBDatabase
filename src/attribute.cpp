#include "attribute.h"

#include "entitytype.h"
#include "row.h"
#include "storage.h"

namespace LBDatabase {

/******************************************************************************
** AttributePrivate
*/
namespace {
const QString NameColumn("name");
const QString EntityTypeIdColumn("entityTypeId");
}

class AttributePrivate {
    AttributePrivate() {}

    void init();

    Row *row;
    Storage *storage;
    QString name;
    EntityType *type;

    Attribute * q_ptr;
    Q_DECLARE_PUBLIC(Attribute)
};

void AttributePrivate::init()
{
    Q_Q(Attribute);
    name = row->data(NameColumn).toString();
    type = storage->entityType(row->data(EntityTypeIdColumn).toInt());
    type->addAttribute(q);
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

} // namespace LBDatabase
