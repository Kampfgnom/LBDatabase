#ifndef LBDATABASE_STORAGE_H
#define LBDATABASE_STORAGE_H

#include <QObject>

#include <QMetaType>

namespace LBDatabase {

class Attribute;
class Context;
class Database;
class EntityType;
class Relation;
class Table;

class StoragePrivate;
class Storage : public QObject
{
    Q_OBJECT
public:
    static QList<Storage*> instances();
    static Storage *instance(const QString &fileName);

    ~Storage();

    bool open();

    Database *database() const;
    QString name() const;
    void setName(const QString &name);
    QString fileName() const;

    QList<Context *> contexts() const;

    Context *addContext(const QString &name, const QString &baseEntityTypeName);

Q_SIGNALS:
    void nameChanged(QString name);

private:
    friend class EntityTypePrivate;
    friend class ContextPrivate;
    friend class EntityPrivate;
    friend class AttributePrivate;
    friend class RelationPrivate;

    explicit Storage(const QString &fileName, QObject *parent = 0);

    Context *context(int id) const;
    EntityType *entityType(int id) const;
    Attribute *attribute(int id) const;

    void insertEntityType(EntityType *type);
    void insertAttribute(Attribute *attribute);
    void insertRelation(Relation *relation);

    Table *contextsTable() const;
    Table *entitiesTable() const;
    Table *attributesTable() const;

    QScopedPointer<StoragePrivate> d_ptr;
    Q_DECLARE_PRIVATE(Storage)
    Q_DISABLE_COPY(Storage)
};

} // namespace LBDatabase

Q_DECLARE_METATYPE(LBDatabase::Storage*)

#endif // LBDATABASE_STORAGE_H
