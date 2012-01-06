#ifndef LBDATABASE_PROPERTY_H
#define LBDATABASE_PROPERTY_H

#include <QObject>

namespace LBDatabase {

class Context;

class Property : public QObject
{
    Q_OBJECT
public:
    explicit Property(QObject *parent = 0);

    virtual int id() const = 0;
    virtual QString displayName(const Context *context = 0) const = 0;

private:
    friend class StoragePrivate;

    virtual void addPropertyValueToEntities() = 0;
};

} // namespace LBDatabase

#endif // LBDATABASE_PROPERTY_H
