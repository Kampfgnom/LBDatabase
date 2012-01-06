#include "storage.h"

#include "attribute.h"
#include "context.h"
#include "database.h"
#include "entity.h"
#include "entitytype.h"
#include "propertyvalue.h"
#include "relation.h"
#include "row.h"
#include "table.h"

#include <QFile>
#include <QMutex>

#include <QDebug>

namespace LBDatabase {

/******************************************************************************
** StoragePrivate
*/
namespace {
const QString MetaDataTableName("lbmeta");
const QString ContextsTableName("lbmeta_contexts");
const QString EntitiesTableName("lbmeta_entitytypes");
const QString AttributesTableName("lbmeta_attributes");
const QString RelationsTableName("lbmeta_relations");

const QString NameColumn("name");
}

class StoragePrivate {
    static QHash<QString, Storage*> instances;

    StoragePrivate() : database(0) {}

    void init();
    bool open();

    Table *attributesTable;
    Table *contextsTable;
    Table *entitiesTable;
    Table *metaDataTable;
    Table *relationsTable;

    QString name;
    QString fileName;
    Database *database;

    QHash<int, Context *> contexts;
    QHash<int, EntityType *> entityTypes;
    QHash<int, Attribute *> attributes;
    QHash<int, Relation *> relations;
    QList<Property *> properties;

    Storage * q_ptr;
    Q_DECLARE_PUBLIC(Storage)
};

QHash<QString, Storage*> StoragePrivate::instances = QHash<QString, Storage*>();

void StoragePrivate::init()
{
    database = Database::instance(fileName);
}

bool StoragePrivate::open()
{
    Q_Q(Storage);
    if(!database)
        return false;

    QFile file(fileName);
    if(!file.exists()) {
        file.open(QFile::ReadOnly);
        file.close();
    }

    if(!database->open())
        return false;

    metaDataTable = database->table(MetaDataTableName);
    if(!metaDataTable)
        return false;

    contextsTable = database->table(ContextsTableName);
    if(!contextsTable)
        return false;

    entitiesTable = database->table(EntitiesTableName);
    if(!entitiesTable)
        return false;

    attributesTable = database->table(AttributesTableName);
    if(!attributesTable)
        return false;

    relationsTable = database->table(RelationsTableName);
    if(!relationsTable)
        return false;

    Row *metaDataRow = metaDataTable->rowAt(0);
    name = metaDataRow->data(NameColumn).toString();

    contexts.reserve(contextsTable->rows().size());
    entityTypes.reserve(entitiesTable->rows().size());
    attributes.reserve(attributesTable->rows().size());
    properties.reserve(attributesTable->rows().size() + relationsTable->rows().size());

    foreach(Row *row, contextsTable->rows()) {
        Context *context = new Context(row, q);
        contexts.insert(row->id(), context);
    }

    foreach(Row *row, entitiesTable->rows()) {
        q->insertEntityType(new EntityType(row, q));
    }

    foreach(Row *row, attributesTable->rows()) {
        Attribute *attribute = new Attribute(row, q);
        attributes.insert(row->id(), attribute);
        properties.append(attribute);
    }

    foreach(Row *row, relationsTable->rows()) {
        Relation *relation = new Relation(row, q);
        relations.insert(row->id(), relation);
        properties.append(relation);
    }

    foreach(Context *context, contexts.values()) {
        context->initializeEntityHierarchy();
        context->loadEntities();
    }

    foreach(Property *property, properties) {
        property->addPropertyValueToEntities();
    }

    foreach(Context *context, contexts.values()) {
        foreach(Entity *entity, context->entities()) {
            foreach(PropertyValue *value, entity->propertieValues()) {
                value->fetchValue();
            }
        }
    }

    return true;
}

/******************************************************************************
** Storage
*/
Storage *Storage::instance(const QString &fileName)
{
    static QMutex mutex(QMutex::Recursive);
    static QObject guard;
    QMutexLocker locker(&mutex);
    Q_UNUSED(locker)

    if(StoragePrivate::instances.contains(fileName)) {
        return StoragePrivate::instances.value(fileName);
    }

    Storage* storage = new Storage(fileName, &guard);
    StoragePrivate::instances.insert(fileName, storage);
    return storage;
}

Storage::~Storage()
{
}

Database *Storage::database() const
{
    Q_D(const Storage);
    return d->database;
}

QString Storage::name() const
{
    Q_D(const Storage);
    return d->name;
}

Storage::Storage(const QString &fileName, QObject *parent) :
    QObject(parent),
    d_ptr(new StoragePrivate)
{
    Q_D(Storage);
    d->q_ptr = this;
    d->fileName = fileName;
    d->init();
}

QString Storage::fileName() const
{
    Q_D(const Storage);
    return d->fileName;
}

EntityType *Storage::entityType(int id) const
{
    Q_D(const Storage);
    return d->entityTypes.value(id, 0);
}

Context *Storage::context(int id) const
{
    Q_D(const Storage);
    return d->contexts.value(id);
}

QList<Context *> Storage::contexts() const
{
    Q_D(const Storage);
    return d->contexts.values();
}

Context *Storage::addContext(const QString &name, const QString &baseEntityTypeName)
{
    qWarning() << "Storage::createContext: IMPLEMENT ME";
}

Attribute *Storage::attribute(int id) const
{
    Q_D(const Storage);
    return d->attributes.value(id);
}

void Storage::insertEntityType(EntityType *type)
{
    Q_D(Storage);
    d->entityTypes.insert(type->id(), type);
}

Table *Storage::entitiesTable() const
{
    Q_D(const Storage);
    return d->entitiesTable;
}

bool Storage::open()
{
    Q_D(Storage);
    return d->open();
}

} // namespace LBDatabase
