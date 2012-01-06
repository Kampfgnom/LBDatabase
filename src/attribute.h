#ifndef LBDATABASE_ATTRIBUTE_H
#define LBDATABASE_ATTRIBUTE_H

#include "property.h"

namespace LBDatabase {

class EntityType;
class Row;
class Storage;

class AttributePrivate;
class Attribute : public Property
{
    Q_OBJECT
public:
    enum PrefetchStrategy {
        NoPrefretch,
        PrefetchOnStartup
    };

    enum Visibility {
        Visible,
        Hidden
    };

    ~Attribute();

    int id() const;
    QString displayName(const Context *context = 0) const;

    QString name() const;
    PrefetchStrategy prefetchStrategy() const;
    Visibility visibility() const;

private:
    friend class StoragePrivate;

    explicit Attribute(Row *row, Storage *parent);

    virtual void addPropertyValueToEntities();

    QScopedPointer<AttributePrivate> d_ptr;
    Q_DECLARE_PRIVATE(Attribute)
    Q_DISABLE_COPY(Attribute)
};

} // namespace LBDatabase

#endif // LBDATABASE_ATTRIBUTE_H
