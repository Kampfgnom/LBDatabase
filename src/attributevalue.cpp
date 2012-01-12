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
/*!
  \class AttributeValue
  \brief The AttributeValue class represents a value of an Attribute of a
  concrete Entity instance.

  \ingroup highlevel-database-classes

  \todo Document when done.
  */

/*!
  \var AttributeValue::d_ptr
  \internal
  */

/*!
  Creates an AttributeValue instance, which represents a conrete property with
  the Attribute description \a attribute of the Entity \a parent.
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

/*!
  Destroys the attribute value.
  */
AttributeValue::~AttributeValue()
{
}

/*!
  Returns the Entity to which this attribute value belongs.
  */
Entity *AttributeValue::entity() const
{
    Q_D(const AttributeValue);
    return d->entity;
}

/*!
  Returns the value of the attribute.

  The \a role parameter is currently being ignored.
  */
QVariant AttributeValue::data(int role) const
{
    Q_UNUSED(role);
    Q_D(const AttributeValue);
    return d->data;
}

/*!
  Sets the value of the attribute to \a data.

  Does nothing if the attribute is not editable.
  */
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

/*!
  Returns true.
  */
bool AttributeValue::isEditable() const
{
    return true;
}

/*!
  Returns the Property instance, which describes this attribute value.
  */
Property *AttributeValue::property() const
{
    Q_D(const AttributeValue);
    return d->attribute;
}

/*!
  Reads the value of the attribute from the database.
  */
void AttributeValue::fetchValue()
{
    Q_D(AttributeValue);
    d->fetchValue();
}

} // namespace LBDatabase
