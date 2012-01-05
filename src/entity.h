#ifndef LBDATABASE_ENTITY_H
#define LBDATABASE_ENTITY_H

#include <QObject>

namespace LBDatabase {

class Attribute;
class AttributeValue;
class Context;
class EntityType;
class Property;
class PropertyValue;
class Row;
class Storage;

class EntityPrivate;
class Entity : public QObject
{
    Q_OBJECT
public:
    explicit Entity(Row *row, Context *parent);
    ~Entity();

    virtual QString displayName(int role = Qt::DisplayRole) const;
    QVariant data(Property *property) const;

    EntityType *entityType() const;
    Storage *storage() const;
    Context *context() const;
    QList<PropertyValue *> propertieValues() const;
    PropertyValue *propertyValue(Property *property) const;

private:
    friend class AttributeValuePrivate;
    friend class Context;
    friend class ContextPrivate;
    friend class RelationValuePrivate;
    friend class RelationInverseValuePrivate;

    Row *row() const;

    void initializeRelations();
    void initializeRelationContent();

    EntityPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(Entity)
};

} // namespace LBDatabase

#endif // LBDATABASE_ENTITY_H
