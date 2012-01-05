#ifndef RELATIONINVERSEVALUE_H
#define RELATIONINVERSEVALUE_H

#include "propertyvalue.h"

namespace LBDatabase {

class Relation;
class Entity;

class RelationInverseValuePrivate;
class RelationInverseValue : public PropertyValue
{
    Q_OBJECT
public:
    explicit RelationInverseValue(Relation *relation, Entity *parent);
    ~RelationInverseValue();

    QVariant data(int role = Qt::DisplayRole) const;

    Property *property() const;
private:
    friend class EntityPrivate;

    void initRelationContent();

    RelationInverseValuePrivate * const d_ptr;
    Q_DECLARE_PRIVATE(RelationInverseValue)
};

} // namespace LBDatabase
#endif // RELATIONINVERSEVALUE_H
