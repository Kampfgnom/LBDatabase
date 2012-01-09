#ifndef LBDATABASE_RELATION_H
#define LBDATABASE_RELATION_H

#include "property.h"

namespace LBDatabase {

class Context;
class EntityType;
class Row;
class Storage;
class Table;

class RelationPrivate;
class Relation : public Property
{
    Q_OBJECT
public:
    enum Direction {
        LeftToRight,
        RightToLeft,
        Both
    };

    enum Cardinality {
        OneToOne,
        OneToMany,
        ManyToMany
    };

    ~Relation();

    int id() const;
    QString displayName(const Context *context = 0) const;

    QString name() const;
    EntityType *entityTypeLeft() const;
    EntityType *entityTypeRight() const;
    Cardinality cardinality() const;
    Direction direction() const;

    Table *relationTable() const;

private:
    friend class StoragePrivate;
    friend class RelationValueRightPrivate;

    explicit Relation(Row *row, Storage *parent);

    void addPropertyValueToEntities();
    void addPropertyValue(Entity *entity);

    void initializeManyToManyRelation();

    QScopedPointer<RelationPrivate> d_ptr;
    Q_DECLARE_PRIVATE(Relation)
    Q_DISABLE_COPY(Relation)
};

} // namespace LBDatabase
#endif // LBDATABASE_RELATION_H
