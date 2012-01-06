#ifndef LBDATABASE_PROPERTYVALUE_H
#define LBDATABASE_PROPERTYVALUE_H

#include <QObject>

namespace LBDatabase {

class Property;

class PropertyValue : public QObject
{
    Q_OBJECT
public:
    explicit PropertyValue(QObject *parent = 0);

    virtual Property *property() const = 0;
    virtual QVariant data(int role = Qt::DisplayRole) const = 0;

private:
    friend class StoragePrivate;

    virtual void fetchValue() = 0;
};

} // namespace LBDatabase

#endif // LBDATABASE_PROPERTYVALUE_H
