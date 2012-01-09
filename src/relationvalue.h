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
    ~RelationValue();

    virtual Entity *entity() const;
    Property *property() const;

    QList<Entity *> entities() const;

    bool isEditable() const;

protected:
    friend class RelationPrivate;

    explicit RelationValue(RelationValuePrivate &dd, Relation *relation, Entity *parent);

    void fetchValue();

    QScopedPointer<RelationValuePrivate> d_ptr;
    Q_DECLARE_PRIVATE(RelationValue)
    Q_DISABLE_COPY(RelationValue)
};

} // namespace LBDatabase

#endif // LBDATABASE_RELATIONVALUE_H
