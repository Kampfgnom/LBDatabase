#ifndef LBDATABASE_ATTRIBUTE_H
#define LBDATABASE_ATTRIBUTE_H

#include <QObject>

namespace LBDatabase {

class Row;
class AttributeMetaData;

class Attribute : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString displayName READ displayName)
    Q_PROPERTY(bool editable READ isEditable)

public:
    explicit Attribute(const QString &name, Row *owner);

    QString name() const;
    QString displayName() const;
    bool isEditable() const;

    AttributeMetaData *metaData() const;
    Row *owner() const;

    virtual QVariant data(int role = Qt::DisplayRole) const = 0;
    virtual void setData(const QVariant &value) = 0;

protected:
    Row *m_owner;
    AttributeMetaData *m_metaData;
};

} // namespace LBDatabase

#endif // LBDATABASE_ATTRIBUTE_H
