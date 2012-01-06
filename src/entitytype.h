#ifndef ENTITYTYPE_H
#define ENTITYTYPE_H

#include <QObject>

namespace LBDatabase {

class Attribute;
class Context;
class Entity;
class Property;
class Relation;
class Row;
class Storage;

class EntityTypePrivate;
class EntityType : public QObject
{
    Q_OBJECT
public:
    ~EntityType();

    QString name() const;
    Context *context() const;
    EntityType *parentEntityType() const;
    int parentEntityTypeId() const;

    QList<EntityType *> childEntityTypes() const;

    QList<Property *> properties() const;
    QList<Attribute *> attributes() const;
    QList<Relation *> relations() const;

    QList<Entity *> entities() const;

    bool inherits(EntityType *entityType) const;

private:
    friend class ContextPrivate;
    friend class StoragePrivate;
    friend class AttributePrivate;
    friend class RelationPrivate;
    friend class EntityPrivate;

    explicit EntityType(LBDatabase::Row *row, Storage *parent);

    void setName(const QString &name);
    void setContext(Context *context);
    void addChildEntityType(EntityType *type);
    void setParentEntityType(EntityType *type);
    void setParentEntityTypeId(int id);
    void addAttribute(Attribute *attribute);
    void addRelation(Relation *relation);
    void addPropertiesToChildren();
    void addEntity(Entity *entity);

    QScopedPointer<EntityTypePrivate> d_ptr;
    Q_DECLARE_PRIVATE(EntityType)
    Q_DISABLE_COPY(EntityType)
};

} // namespace LBDatabase

#endif // ENTITYTYPE_H
