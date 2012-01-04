#include "storage.h"

#include "context.h"
#include "database.h"
#include "entitytype.h"
#include "row.h"
#include "table.h"

#include <QFile>
#include <QMutex>

#include <QDebug>

namespace LBDatabase {

namespace {
const QString EntitiesTableName("lbmeta_entitytypes");
}

/******************************************************************************
** StoragePrivate
*/
class StoragePrivate {
    static QHash<QString, Storage*> instances;

    StoragePrivate() : database(0) {}

    void init();
    bool open();

    QString fileName;
    Database *database;

    Table *entitiesTable;
    QHash<QString, Context *> contexts;

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

    entitiesTable = database->table(EntitiesTableName);
    if(!entitiesTable)
        return false;

    foreach(Row *row, entitiesTable->rows()) {
        QString contextName = row->data(QLatin1String("context")).toString();
        if(!contexts.contains(contextName)) {
            contexts.insert(contextName, new Context(contextName, q));
        }

        Context *context = contexts.value(contextName);

        EntityType *type = new EntityType(context);
        type->setName(row->data(QLatin1String("name")).toString());
        type->setParentEntityTypeName(row->data(QLatin1String("parentEntityType")).toString());
        context->addEntityType(type);
    }

    foreach(Context *context, contexts.values()) {
        context->init();
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

Context *Storage::context(const QString &name) const
{
    Q_D(const Storage);
    return d->contexts.value(name);
}

QList<Context *> Storage::contexts() const
{
    Q_D(const Storage);
    return d->contexts.values();
}

bool Storage::open()
{
    Q_D(Storage);
    return d->open();
}

} // namespace LBDatabase
