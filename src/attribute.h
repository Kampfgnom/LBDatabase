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

    enum Type {
        Simple,
        Relation
    };

    explicit Attribute(Row *row, Storage *parent);
    ~Attribute();

    int id() const;
    QString name() const;
    QString displayName(const Context *context = 0) const;
    PrefetchStrategy prefetchStrategy() const;
    Visibility visibility() const;

Q_SIGNALS:
    void nameChanged(QString name);

private:
    friend class StoragePrivate;

    void setName(const QString &name);

    AttributePrivate * const d_ptr;
    Q_DECLARE_PRIVATE(Attribute)
};

} // namespace LBDatabase

#endif // LBDATABASE_ATTRIBUTE_H
