#ifndef LBDATABASE_STORAGE_H
#define LBDATABASE_STORAGE_H

#include <QObject>

namespace LBDatabase {

class Context;
class Database;
class EntityType;

class StoragePrivate;
class Storage : public QObject
{
    Q_OBJECT
public:
    static QList<Storage*> instances();
    static Storage *instance(const QString &fileName);

    ~Storage();

    Database *database() const;

    void setFileName(const QString &fileName);
    QString fileName() const;

    EntityType *entityType(int id) const;

    Context *context(int id) const;
    Context *context(const QString &name) const;
    QList<Context *> contexts() const;

    bool open();

private:
    explicit Storage(QObject *parent = 0);
    explicit Storage(const QString &fileName, QObject *parent = 0);

    StoragePrivate * const d_ptr;
    Q_DECLARE_PRIVATE(Storage)
};

} // namespace LBDatabase

#endif // LBDATABASE_STORAGE_H
