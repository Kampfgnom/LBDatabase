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
class RelationValue;
class Row;
class Storage;

class EntityPrivate;
class Entity : public QObject
{
    Q_OBJECT
public:
    ~Entity();

    virtual QString displayName(int role = Qt::DisplayRole) const;
    QVariant data(Property *property) const;

    EntityType *entityType() const;
    Storage *storage() const;
    Context *context() const;
    QList<PropertyValue *> propertieValues() const;
    PropertyValue *propertyValue(Property *property) const;

    Row *row() const;

private:
    friend class AttributePrivate;
    friend class ContextPrivate;
    friend class RelationPrivate;

    explicit Entity(Row *row, Context *parent);

    void addAttributeValue(AttributeValue *value);
    void addRelationValue(RelationValue *value);

    QScopedPointer<EntityPrivate> d_ptr;
    Q_DECLARE_PRIVATE(Entity)
    Q_DISABLE_COPY(Entity)
};

} // namespace LBDatabase

#endif // LBDATABASE_ENTITY_H
