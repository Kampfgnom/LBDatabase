#ifndef LBDATABASE_ENTITY_H
#define LBDATABASE_ENTITY_H

#include <QObject>

namespace LBDatabase {

class Attribute;
class EntityType;
class Row;
class Storage;

class EntityPrivate;
class Entity : public QObject
{
    Q_OBJECT
public:
    explicit Entity(Row *row, Storage *parent);
    ~Entity();

    QVariant value(Attribute *attribute) const;

    EntityType *entityType() const;

private:
    friend class AttributeValuePrivate;
    friend class Context;

    QVariant data(const QString &attributeName) const;
    Row *row() const;

    EntityPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(Entity)
};

} // namespace LBDatabase

#endif // LBDATABASE_ENTITY_H
