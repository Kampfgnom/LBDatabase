#ifndef LBDATABASE_CONTEXT_H
#define LBDATABASE_CONTEXT_H

#include <QAbstractTableModel>

#include <QMetaType>

namespace LBDatabase {

class Attribute;
class Entity;
class EntityType;
class Relation;
class Row;
class Storage;

class ContextPrivate;
class Context : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit Context(Row *row, Storage *parent);
    ~Context();

    QString name() const;
    Storage *storage() const;

    EntityType *baseEntityType() const;
    EntityType *entityType(const QString &name) const;
    QList<EntityType *> entityTypes() const;

    Entity *entity(int id) const;
    Entity *entityAt(int index) const;
    QList<Entity *> entities() const;

    // QAbstractTableModel
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
//    bool setData(const QModelIndex &index, const QVariant &value, int role);
//    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    friend class StoragePrivate;
    friend class EntityTypePrivate;
    friend class AttributePrivate;
    friend class RelationPrivate;

    void addEntityType(EntityType *type);
    void addAttribute(Attribute *attribute);
    void addRelation(Relation *relation);

    void initializeEntityHierarchy();
    void loadEntities();
    void initializeRelations();

    ContextPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(Context)
};

} // namespace LBDatabase

Q_DECLARE_METATYPE(LBDatabase::Context*)

#endif // LBDATABASE_CONTEXT_H
