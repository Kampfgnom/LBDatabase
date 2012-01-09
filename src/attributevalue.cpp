#include "attributevalue.h"

#include "attribute.h"
#include "context.h"
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
    Q_Q(AttributeValue);
    QObject::connect(q, SIGNAL(dataChanged(QVariant)), entity->context(), SLOT(onPropertyValueDataChanged(QVariant)));
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

Entity *AttributeValue::entity() const
{
    Q_D(const AttributeValue);
    return d->entity;
}

QVariant AttributeValue::data(int role) const
{
    Q_UNUSED(role);
    Q_D(const AttributeValue);
    return d->data;
}

bool AttributeValue::setData(const QVariant &data)
{
    Q_D(AttributeValue);
    if(!isEditable())
        return false;

    d->entity->row()->setData(d->attribute->name(), data);
    d->data = data;
    emit dataChanged(data);
    return true;
}

bool AttributeValue::isEditable() const
{
    return true;
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
