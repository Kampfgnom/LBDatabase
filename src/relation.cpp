#include "relation.h"

#include "attribute.h"
#include "context.h"
#include "entity.h"
#include "entitytype.h"
#include "row.h"
#include "storage.h"

namespace LBDatabase {

/******************************************************************************
** RelationPrivate
*/
namespace {
const QString NameColumn("name");
const QString DisplayNameLeftColumn("displaynameleft");
const QString DisplayNameRightColumn("displaynameright");
const QString EntityTypeLeftColumn("entitytypeleft");
const QString EntityTypeRightColumn("entitytyperight");
const QString CardinalityColumn("cardinality");
const QString DirectionColumn("direction");
}

class RelationPrivate {
    RelationPrivate() :
        entityTypeLeft(0), entityTypeRight(0),
        cardinality(Relation::OneToOne),
        direction(Relation::LeftToRight)
    {}

    void init();

    Row *row;
    Storage *storage;

    QString name;
    QString displayNameLeft;
    QString displayNameRight;
    EntityType *entityTypeLeft;
    EntityType *entityTypeRight;
    Relation::Cardinality cardinality;
    Relation::Direction direction;

    Relation * q_ptr;
    Q_DECLARE_PUBLIC(Relation)
};

void RelationPrivate::init()
{
    Q_Q(Relation);

    name = row->data(NameColumn).toString();
    displayNameLeft = row->data(DisplayNameLeftColumn).toString();
    displayNameRight = row->data(DisplayNameRightColumn).toString();
    entityTypeLeft = storage->entityType(row->data(EntityTypeLeftColumn).toInt());
    entityTypeRight = storage->entityType(row->data(EntityTypeRightColumn).toInt());
    cardinality = static_cast<Relation::Cardinality>(row->data(CardinalityColumn).toInt());
    direction = static_cast<Relation::Direction>(row->data(DirectionColumn).toInt());

    if(entityTypeLeft) {
        entityTypeLeft->addRelation(q);
        entityTypeLeft->context()->addRelation(q);
    }
    if(entityTypeRight) {
        entityTypeRight->addRelation(q);
        entityTypeRight->context()->addRelation(q);
    }
}

/******************************************************************************
** Relation
*/
Relation::Relation(Row *row, Storage *parent) :
    Property(parent),
    d_ptr(new RelationPrivate)
{
    Q_D(Relation);
    d->q_ptr = this;
    d->row = row;
    d->storage = parent;
    d->init();
}

Relation::~Relation()
{
    Q_D(Relation);
    delete d;
}

int Relation::id() const
{
    Q_D(const Relation);
    return d->row->id();
}

QString Relation::name() const
{
    Q_D(const Relation);
    return d->name;
}

QString Relation::displayName(const Context *context) const
{
    Q_D(const Relation);
    if(context) {
        if(d->entityTypeLeft && context == d->entityTypeLeft->context())
            return d->displayNameLeft;
        if(d->entityTypeRight && context == d->entityTypeRight->context())
            return d->displayNameRight;
    }

    return d->displayNameLeft;
}

EntityType *Relation::entityTypeLeft() const
{
    Q_D(const Relation);
    return d->entityTypeLeft;
}

EntityType *Relation::entityTypeRight() const
{
    Q_D(const Relation);
    return d->entityTypeRight;
}

Relation::Cardinality Relation::cardinality() const
{
    Q_D(const Relation);
    return d->cardinality;
}

Relation::Direction Relation::direction() const
{
    Q_D(const Relation);
    return d->direction;
}

} // namespace LBDatabase
