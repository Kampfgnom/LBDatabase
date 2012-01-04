#ifndef LBDATABASE_CONTEXT_H
#define LBDATABASE_CONTEXT_H

#include <QObject>

namespace LBDatabase {

class EntityType;
class Row;
class Storage;

class ContextPrivate;
class Context : public QObject
{
    Q_OBJECT
public:
    explicit Context(Row *row, Storage *parent);
    ~Context();

    QString name() const;

    EntityType *baseEntityType() const;
    EntityType *entityType(const QString &name) const;
    QList<EntityType *> entityTypes() const;

private:
    friend class StoragePrivate;
    friend class EntityTypePrivate;

    void addEntityType(EntityType *type);
    void initializeEntityHierarchy();

    ContextPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(Context)
};

} // namespace LBDatabase

#endif // LBDATABASE_CONTEXT_H
