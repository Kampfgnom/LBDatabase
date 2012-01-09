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
    static const QString NameColumn;

    ~Context();

    int id() const;
    QString name() const;
//    void setName(const QString &name);
    Storage *storage() const;

    EntityType *baseEntityType() const;
    QList<EntityType *> entityTypes() const;
    EntityType *addEntityType(const QString &name, EntityType *parentEntityType);

    Entity *entity(int id) const;
    QList<Entity *> entities() const;
    Entity *insertEntity(EntityType *type);

    // QAbstractTableModel
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;

Q_SIGNALS:
    void nameChanged(QString name);

private Q_SLOTS:
    void onEntityTypeNameChanged(QString name);
    void onPropertyDisplayNameChanged(QString displayName, Context* context);
    void onPropertyValueDataChanged(QVariant data);

private:
    friend class StoragePrivate;
    friend class EntityTypePrivate;
    friend class AttributePrivate;
    friend class RelationPrivate;

    explicit Context(Row *row, Storage *parent);

    void createBaseEntityType(const QString &name);
    void addEntityType(EntityType *type);
    void addAttribute(Attribute *attribute);
    void addRelation(Relation *relation);

    void initializeEntityHierarchy();
    void loadEntities();
    void initializeRelations();
    void fillRelations();

    QScopedPointer<ContextPrivate> d_ptr;
    Q_DECLARE_PRIVATE(Context)
    Q_DISABLE_COPY(Context)
};

} // namespace LBDatabase

Q_DECLARE_METATYPE(LBDatabase::Context*)

#endif // LBDATABASE_CONTEXT_H
