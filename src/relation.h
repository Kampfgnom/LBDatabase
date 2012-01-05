#ifndef LBDATABASE_RELATION_H
#define LBDATABASE_RELATION_H

#include "property.h"

namespace LBDatabase {

class Context;
class EntityType;
class Row;
class Storage;

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

    explicit Relation(Row *row, Storage *parent);
    ~Relation();

    int id() const;
    QString name() const;
    QString displayName(const Context *context = 0) const;
    EntityType *entityTypeLeft() const;
    EntityType *entityTypeRight() const;
    Cardinality cardinality() const;
    Direction direction() const;

private:
    friend class StoragePrivate;

    RelationPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(Relation)
};

} // namespace LBDatabase
#endif // LBDATABASE_RELATION_H
