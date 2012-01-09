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
    static const QString NameColumn;
    static const QString DisplayNameColumn;
    static const QString EntityTypeIdColumn;
    static const QString PrefetchStrategyColumn;

    enum PrefetchStrategy {
        PrefetchOnStartup
    };

    ~Attribute();

    int id() const;
    QString displayName(const Context *context = 0) const;
    virtual void setDisplayName(const QString &displayName, const Context *context = 0);

    QString name() const;
    PrefetchStrategy prefetchStrategy() const;

private:
    friend class StoragePrivate;
    friend class EntityTypePrivate;

    explicit Attribute(Row *row, Storage *parent);

    void addPropertyValueToEntities();
    void addPropertyValue(Entity *entity);

    QScopedPointer<AttributePrivate> d_ptr;
    Q_DECLARE_PRIVATE(Attribute)
    Q_DISABLE_COPY(Attribute)
};

} // namespace LBDatabase

#endif // LBDATABASE_ATTRIBUTE_H
