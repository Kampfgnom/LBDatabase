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
    Context *addContext(const QString &name, const QString &baseEntityTypeName);

    Table *attributesTable;
    Table *contextsTable;
    Table *entityTypesTable;
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

    entityTypesTable = database->table(EntitiesTableName);
    if(!entityTypesTable)
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
    entityTypes.reserve(entityTypesTable->rows().size());
    attributes.reserve(attributesTable->rows().size());
    properties.reserve(attributesTable->rows().size() + relationsTable->rows().size());

    foreach(Row *row, contextsTable->rows()) {
        Context *context = new Context(row, q);
        contexts.insert(row->id(), context);
    }

    foreach(Row *row, entityTypesTable->rows()) {
        q->insertEntityType(new EntityType(row, q));
    }

    foreach(Row *row, attributesTable->rows()) {
        q->insertAttribute(new Attribute(row, q));
    }

    foreach(Row *row, relationsTable->rows()) {
        q->insertRelation(new Relation(row, q));
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
            foreach(PropertyValue *value, entity->propertyValues()) {
                value->fetchValue();
            }
        }
    }

    return true;
}

Context *StoragePrivate::addContext(const QString &name, const QString &baseEntityTypeName)
{
    Q_Q(Storage);
    database->createTable(name);
    Row *row = contextsTable->appendRow();
    row->setData(Context::NameColumn, QVariant(name));

    Context *context = new Context(row, q);
    contexts.insert(row->id(), context);

    context->createBaseEntityType(baseEntityTypeName);

    return context;
}

/******************************************************************************
** Storage
*/
/*!
  \class Storage
  \brief The Storage class represents a high level storage for entities.

  \ingroup highlevel-database-classes

  \todo Dokument
  */

/*!
  \var Storage::d_ptr
  \internal
  */

/*!
  \fn Storage::nameChanged()

  This signal is emitted when the name of this storage changes.

  */

/*!
  Returns a storage instance, which holds the data contained in the storage file
  \a fileName.

  This will return exactly one instance per file, i.e. you can open each file
  only exactly once.
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

/*!
  Closes the storage.
  */
Storage::~Storage()
{
}

/*!
  Returns the database, on which the storage works. Note that changes to the
  database are not automatically reflected in the storage.
  */
Database *Storage::database() const
{
    Q_D(const Storage);
    return d->database;
}

/*!
  Returns the name of the storage.
  */
QString Storage::name() const
{
    Q_D(const Storage);
    return d->name;
}

/*!
  Sets the name of the storage to \a name. Note that this name is not connected
  to the file name, but is stored in the database itself.
  */
void Storage::setName(const QString &name)
{
    Q_D(Storage);
    if(d->name == name)
        return;

    d->metaDataTable->rowAt(0)->setData(NameColumn, QVariant(name));
    d->name = name;
    emit nameChanged(name);
}

/*!
  Creates a storage, which uses the database \a fileName.
  */
Storage::Storage(const QString &fileName, QObject *parent) :
    QObject(parent),
    d_ptr(new StoragePrivate)
{
    Q_D(Storage);
    d->q_ptr = this;
    d->fileName = fileName;
    d->init();
}

/*!
  Returns the file name of the storage.
  */
QString Storage::fileName() const
{
    Q_D(const Storage);
    return d->fileName;
}

/*!
  Returns the EntityType with the ID \a id.
  */
EntityType *Storage::entityType(int id) const
{
    Q_D(const Storage);
    return d->entityTypes.value(id, 0);
}

/*!
  Returns the EntityType with the ID \a id.
  */
Context *Storage::context(int id) const
{
    Q_D(const Storage);
    return d->contexts.value(id);
}

/*!
  Returns a list of all contexts in this storage.
  */
QList<Context *> Storage::contexts() const
{
    Q_D(const Storage);
    return d->contexts.values();
}

/*!
  Creates a new context in the storage with the name \a name. It will
  automatically create a base entity type for the context named \a
  baseEntityTypeName, since each context has to have exactly one base entity
  type.
  */
Context *Storage::addContext(const QString &name, const QString &baseEntityTypeName)
{
    Q_D(Storage);
    return d->addContext(name, baseEntityTypeName);
}

/*!
  Returns the Attribute with the ID \a id.
  */
Attribute *Storage::attribute(int id) const
{
    Q_D(const Storage);
    return d->attributes.value(id, 0);
}

/*!
  \internal

  Inserts the entity type \a type into the storage-global list of types.
  */
void Storage::insertEntityType(EntityType *type)
{
    Q_D(Storage);
    if(d->entityTypes.contains(type->id()))
        return;

    d->entityTypes.insert(type->id(), type);
}

/*!
  \internal

  Inserts the attribute \a attribute into the storage-global list of attributes.
  */
void Storage::insertAttribute(Attribute *attribute)
{
    Q_D(Storage);
    if(d->attributes.contains(attribute->id()))
        return;

    d->attributes.insert(attribute->id(), attribute);
    d->properties.append(attribute);
}

/*!
  \internal

  Inserts the relation \a relation into the storage-global list of relations.
  */
void Storage::insertRelation(Relation *relation)
{
    Q_D(Storage);
    if(d->relations.contains(relation->id()))
        return;

    d->relations.insert(relation->id(), relation);
    d->properties.append(relation);
}

/*!
  Returns the table, which defines all contexts.
  */
Table *Storage::contextsTable() const
{
    Q_D(const Storage);
    return d->contextsTable;
}

/*!
  Returns the table, which defines all entity types.
  */
Table *Storage::entityTypesTable() const
{
    Q_D(const Storage);
    return d->entityTypesTable;
}

/*!
  Returns the table, which defines all attributes.
  */
Table *Storage::attributesTable() const
{
    Q_D(const Storage);
    return d->attributesTable;
}

/*!
  Opens the storage.
  Returns true upon success and false if something goes wrong (e.g. the file is
  no correct storage).
  */
bool Storage::open()
{
    Q_D(Storage);
    return d->open();
}

/*!
  Returns a list of all entity types, that this storage may contain.
  */
QList<EntityType *> Storage::entityTypes() const
{
    Q_D(const Storage);
    return d->entityTypes.values();
}

} // namespace LBDatabase
