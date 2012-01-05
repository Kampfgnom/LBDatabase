#include "attributevalue.h"

#include "attribute.h"
#include "entity.h"

#include <QVariant>

namespace LBDatabase {

/******************************************************************************
** AttributeValuePrivate
*/
class AttributeValuePrivate {
    AttributeValuePrivate() {}

    void init();

    Entity *entity;
    Attribute *attribute;

    QVariant value;

    AttributeValue * q_ptr;
    Q_DECLARE_PUBLIC(AttributeValue)
};

void AttributeValuePrivate::init()
{
    Q_Q(AttributeValue);

    if(attribute->prefetchStrategy() == Attribute::PrefetchOnStartup) {
        value = entity->data(attribute->name());
    }
}

/******************************************************************************
** AttributeValue
*/
AttributeValue::AttributeValue(Attribute *attribute, Entity *parent) :
    QObject(parent),
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
    Q_D(AttributeValue);
    delete d;
}

QVariant AttributeValue::value() const
{
    Q_D(const AttributeValue);
    return d->value;
}

} // namespace LBDatabase
