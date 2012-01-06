#include "attributevalue.h"

#include "attribute.h"
#include "entity.h"
#include "row.h"

#include <QVariant>

namespace LBDatabase {

/******************************************************************************
** AttributeValuePrivate
*/
class AttributeValuePrivate {
    AttributeValuePrivate() {}

    void init();
    void fetchValue();

    Entity *entity;
    Attribute *attribute;

    QVariant data;

    AttributeValue * q_ptr;
    Q_DECLARE_PUBLIC(AttributeValue)
};

void AttributeValuePrivate::init()
{
}

void AttributeValuePrivate::fetchValue()
{
    data = entity->row()->data(attribute->name());
}

/******************************************************************************
** AttributeValue
*/
AttributeValue::AttributeValue(Attribute *attribute, Entity *parent) :
    PropertyValue(parent),
    d_ptr(new AttributeValuePrivate)
{
    Q_D(AttributeValue);
    d->q_ptr = this;
    d->attribute = attribute;
    d->entity = parent;
    d->init();
}

AttributeValue::~AttributeValue()
{
}

QVariant AttributeValue::data(int role) const
{
    Q_UNUSED(role)
    Q_D(const AttributeValue);
    return d->data;
}

Property *AttributeValue::property() const
{
    Q_D(const AttributeValue);
    return d->attribute;
}

void AttributeValue::fetchValue()
{
    Q_D(AttributeValue);
    d->fetchValue();
}

} // namespace LBDatabase
