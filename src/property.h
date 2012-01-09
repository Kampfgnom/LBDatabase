#ifndef LBDATABASE_PROPERTY_H
#define LBDATABASE_PROPERTY_H

#include <QObject>

namespace LBDatabase {

class Context;
class Entity;

class Property : public QObject
{
    Q_OBJECT
public:
    explicit Property(QObject *parent = 0);

    virtual int id() const = 0;
    virtual QString displayName(const Context *context = 0) const = 0;
    virtual void setDisplayName(const QString &displayName, const Context *context = 0) = 0;

Q_SIGNALS:
    void displayNameChanged(QString displayName, Context *context);

private:
    friend class StoragePrivate;
    friend class ContextPrivate;

    virtual void addPropertyValueToEntities() = 0;
    virtual void addPropertyValue(Entity *entity) = 0;
};

} // namespace LBDatabase

#endif // LBDATABASE_PROPERTY_H
