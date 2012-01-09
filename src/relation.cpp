#include "relation.h"

#include "attribute.h"
#include "context.h"
#include "database.h"
#include "entity.h"
#include "entitytype.h"
#include "relationvalue_p.h"
#include "relationvalueleft.h"
#include "relationvalueright.h"
#include "row.h"
#include "storage.h"
#include "table.h"

namespace LBDatabase {

/******************************************************************************
** RelationPrivate
*/
const QString Relation::NameColumn("name");
const QString Relation::DisplayNameLeftColumn("displaynameleft");
const QString Relation::DisplayNameRightColumn("displaynameright");
const QString Relation::EntityTypeLeftColumn("entitytypeleft");
const QString Relation::EntityTypeRightColumn("entitytyperight");
const QString Relation::CardinalityColumn("cardinality");

class RelationPrivate {
    RelationPrivate() :
        entityTypeLeft(0), entityTypeRight(0),
        cardinality(Relation::OneToOne),
        relationTable(0)
    {}

    void init();
    void addPropertyValueToEntities();
    void initializeManyToManyRelation();
    void addPropertyValue(Entity *entity);

    Row *row;
    Storage *storage;

    QString name;
    QString displayNameLeft;
    QString displayNameRight;
    EntityType *entityTypeLeft;
    EntityType *entityTypeRight;
    Relation::Cardinality cardinality;

    Table *relationTable;

    Relation * q_ptr;
    Q_DECLARE_PUBLIC(Relation)
};

void RelationPrivate::init()
{
    Q_Q(Relation);

    name = row->data(Relation::NameColumn).toString();
    displayNameLeft = row->data(Relation::DisplayNameLeftColumn).toString();
    displayNameRight = row->data(Relation::DisplayNameRightColumn).toString();
    entityTypeLeft = storage->entityType(row->data(Relation::EntityTypeLeftColumn).toInt());
    entityTypeRight = storage->entityType(row->data(Relation::EntityTypeRightColumn).toInt());
    cardinality = static_cast<Relation::Cardinality>(row->data(Relation::CardinalityColumn).toInt());

    relationTable = storage->database()->table(name);

    if(entityTypeLeft) {
        entityTypeLeft->addRelation(q);
        entityTypeLeft->context()->addRelation(q);
    }
    if(entityTypeRight) {
        entityTypeRight->addRelation(q);
        entityTypeRight->context()->addRelation(q);
    }
}

void RelationPrivate::addPropertyValueToEntities()
{
    Q_Q(Relation);
    foreach(Entity *entity, entityTypeLeft->entities()) {
        entity->addRelationValue(new RelationValueLeft(q, entity));
    }
    foreach(Entity *entity, entityTypeRight->entities()) {
        entity->addRelationValue(new RelationValueRight(q, entity));
    }
}

void RelationPrivate::initializeManyToManyRelation()
{
    Q_Q(Relation);
    foreach(Row *row, relationTable->rows()) {
        int leftId = row->data(entityTypeLeft->name()).toInt();
        int rightId = row->data(entityTypeRight->name()).toInt();
        Entity *leftEntity = entityTypeLeft->context()->entity(leftId);
        Entity *rightEntity = entityTypeRight->context()->entity(rightId);

        RelationValuePrivate *leftValue = qobject_cast<RelationValue *>(leftEntity->propertyValue(q))->d_func();
        RelationValuePrivate *rightValue = qobject_cast<RelationValue *>(rightEntity->propertyValue(q))->d_func();
        leftValue->addOtherEntity(rightEntity);
        rightValue->addOtherEntity(leftEntity);
    }
}

void RelationPrivate::addPropertyValue(Entity *entity)
{
    Q_Q(Relation);
    if(entity->entityType() == entityTypeLeft) {
        entity->addRelationValue(new RelationValueLeft(q, entity));
    }
    else if(entity->entityType() == entityTypeLeft) {
        entity->addRelationValue(new RelationValueRight(q, entity));
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
        else if(d->entityTypeRight && context == d->entityTypeRight->context())
            return d->displayNameRight;
    }

    return d->name;
}

void Relation::setDisplayName(const QString &displayName, const Context *context)
{
    Q_D(Relation);
    if(context) {
        if(d->entityTypeLeft && context == d->entityTypeLeft->context()) {
            d->row->setData(DisplayNameLeftColumn, QVariant(displayName));
            emit displayNameChanged(displayName, d->entityTypeLeft->context());
        }
        else if(d->entityTypeRight && context == d->entityTypeRight->context()) {
            d->row->setData(DisplayNameRightColumn, QVariant(displayName));
            emit displayNameChanged(displayName, d->entityTypeRight->context());
        }
    }
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

Table *Relation::relationTable() const
{
    Q_D(const Relation);
    return d->relationTable;
}

void Relation::addPropertyValueToEntities()
{
    Q_D(Relation);
    return d->addPropertyValueToEntities();
}

void Relation::addPropertyValue(Entity *entity)
{
    Q_D(Relation);
    return d->addPropertyValue(entity);
}

void Relation::initializeManyToManyRelation()
{
    Q_D(Relation);
    return d->initializeManyToManyRelation();
}

} // namespace LBDatabase
