#ifndef ENTITYTYPE_H
#define ENTITYTYPE_H

#include <QObject>

namespace LBDatabase {

class Context;

class EntityTypePrivate;
class EntityType : public QObject
{
    Q_OBJECT
public:
    explicit EntityType(QObject *parent = 0);
    explicit EntityType(Context *parent = 0);
    ~EntityType();

    QString name() const;
    Context *context() const;
    EntityType *parentEntityType() const;
    QString parentEntityTypeName() const;

    QList<EntityType *> childEntityTypes() const;

Q_SIGNALS:
    void nameChanged(QString name);

private:
    friend class ContextPrivate;
    friend class StoragePrivate;

    void setName(const QString &name);
    void setContext(Context *context);
    void addChildEntityType(EntityType *type);
    void setParentEntityType(EntityType *type);
    void setParentEntityTypeName(const QString &name);

    EntityTypePrivate * const d_ptr;
    Q_DECLARE_PRIVATE(EntityType)
};

} // namespace LBDatabase

#endif // ENTITYTYPE_H
