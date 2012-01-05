#ifndef ENTITYTYPE_H
#define ENTITYTYPE_H

#include <QObject>

namespace LBDatabase {

class Attribute;
class Context;
class Row;
class Storage;

class EntityTypePrivate;
class EntityType : public QObject
{
    Q_OBJECT
public:
    explicit EntityType(LBDatabase::Row *row, Storage *parent);
    ~EntityType();

    QString name() const;
    Context *context() const;
    EntityType *parentEntityType() const;
    int parentEntityTypeId() const;

    QList<EntityType *> childEntityTypes() const;

    QList<Attribute *> attributes() const;
    QList<Attribute *> aggregatedAttributes() const;

Q_SIGNALS:
    void nameChanged(QString name);

private:
    friend class ContextPrivate;
    friend class StoragePrivate;
    friend class AttributePrivate;

    void setName(const QString &name);
    void setContext(Context *context);
    void addChildEntityType(EntityType *type);
    void setParentEntityType(EntityType *type);
    void setParentEntityTypeId(int id);
    void addAttribute(Attribute *attribute);
    void addAttributesToChildren();

    EntityTypePrivate * const d_ptr;
    Q_DECLARE_PRIVATE(EntityType)
};

} // namespace LBDatabase

#endif // ENTITYTYPE_H
