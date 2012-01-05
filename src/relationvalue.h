#ifndef LBDATABASE_RELATIONVALUE_H
#define LBDATABASE_RELATIONVALUE_H

#include "propertyvalue.h"

namespace LBDatabase {

class Relation;
class Entity;

class RelationValuePrivate;
class RelationValue : public PropertyValue
{
    Q_OBJECT
public:
    explicit RelationValue(Relation *relation, Entity *parent);
    ~RelationValue();

    QVariant data(int role = Qt::DisplayRole) const;

    Property *property() const;
private:
    friend class RelationInverseValuePrivate;
    friend class EntityPrivate;

    void addEntityFromInverse(Entity *entity);
    void initRelationContent();

    RelationValuePrivate * const d_ptr;
    Q_DECLARE_PRIVATE(RelationValue)
};

} // namespace LBDatabase

#endif // LBDATABASE_RELATIONVALUE_H
