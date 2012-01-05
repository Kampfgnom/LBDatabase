#ifndef LBDATABASE_ATTRIBUTEVALUE_H
#define LBDATABASE_ATTRIBUTEVALUE_H

#include <QObject>

namespace LBDatabase {

class Attribute;
class Entity;

class AttributeValuePrivate;
class AttributeValue : public QObject
{
    Q_OBJECT
public:
    explicit AttributeValue(Attribute *attribute, Entity *parent);
    ~AttributeValue();

    QVariant value() const;

private:
    AttributeValuePrivate * const d_ptr;
    Q_DECLARE_PRIVATE(AttributeValue)
};

} // namespace LBDatabase

#endif // LBDATABASE_ATTRIBUTEVALUE_H
