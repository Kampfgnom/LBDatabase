#include "storage.h"

#include "attribute.h"
#include "context.h"
#include "database.h"
#include "entitytype.h"
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
const QString ContextsTableName("lbmeta_contexts");
const QString EntitiesTableName("lbmeta_entitytypes");
const QString AttributesTableName("lbmeta_attributes");
}

class StoragePrivate {
    static QHash<QString, Storage*> instances;

    StoragePrivate() : database(0) {}

    void init();
    bool open();

    Table *attributesTable;
    Table *contextsTable;
    Table *entitiesTable;

    QString fileName;
    Database *database;

    QHash<int, Context *> contexts;
    QHash<QString, Context *> contextsByName;

    QHash<int, EntityType *> entityTypes;

    QHash<int, Attribute *> attributes;

    Storage * q_ptr;
    Q_DECLARE_PUBLIC(Storage)
};

QHash<QString, Storage*> StoragePrivate::instances = QHash<QString, Storage*>();

void StoragePrivate::init()
{
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

    contextsTable = database->table(ContextsTableName);
    if(!contextsTable)
        return false;

    entitiesTable = database->table(EntitiesTableName);
    if(!entitiesTable)
        return false;

    attributesTable = database->table(AttributesTableName);
    if(!attributesTable)
        return false;

    foreach(Row *row, contextsTable->rows()) {
        Context *context = new Context(row, q);
        contextsByName.insert(context->name(), context);
        contexts.insert(row->id(), context);
    }

    foreach(Row *row, entitiesTable->rows()) {
        EntityType *type = new EntityType(row, q);
        entityTypes.insert(row->id(), type);
    }

    foreach(Context *context, contextsByName.values()) {
        context->initializeEntityHierarchy();
    }

    foreach(Row *row, attributesTable->rows()) {
        Attribute *attribute = new Attribute(row, q);
        attributes.insert(row->id(), attribute);
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
    Q_D(Storage);
    delete d;
}

Database *Storage::database() const
{
    Q_D(const Storage);
    return d->database;
}

Storage::Storage(QObject *parent) :
    QObject(parent),
    d_ptr(new StoragePrivate)
{
    Q_D(Storage);
    d->q_ptr = this;
    d->init();
}

Storage::Storage(const QString &fileName, QObject *parent) :
    QObject(parent),
    d_ptr(new StoragePrivate)
{
    Q_D(Storage);
    d->q_ptr = this;
    d->init();
    setFileName(fileName);
}

void Storage::setFileName(const QString &fileName)
{
    Q_D(Storage);
    if(d->fileName == fileName)
        return;

    d->fileName = fileName;
    d->database = Database::instance(fileName);
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

Context *Storage::context(const QString &name) const
{
    Q_D(const Storage);
    return d->contextsByName.value(name);
}

QList<Context *> Storage::contexts() const
{
    Q_D(const Storage);
    return d->contextsByName.values();
}

bool Storage::open()
{
    Q_D(Storage);
    return d->open();
}

} // namespace LBDatabase
