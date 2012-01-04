#ifndef LBDATABASE_ATTRIBUTE_H
#define LBDATABASE_ATTRIBUTE_H

#include <QObject>

namespace LBDatabase {

class EntityType;
class Row;
class Storage;

class AttributePrivate;
class Attribute : public QObject
{
    Q_OBJECT
public:
    explicit Attribute(Row *row, Storage *parent);
    ~Attribute();

    QString name() const;

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
