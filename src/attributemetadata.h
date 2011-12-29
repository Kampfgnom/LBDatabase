#ifndef LBDATABASE_ATTRIBUTEMETADATA_H
#define LBDATABASE_ATTRIBUTEMETADATA_H

#include <QObject>

#include <QSqlQuery>

namespace LBDatabase {

class Attribute;
class Row;
class Table;
class TableMetaData;

class AttributeMetaData : public QObject
{
    Q_OBJECT
    Q_ENUMS(Type)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString displayName READ displayName WRITE setDisplayName NOTIFY displayNameChanged)
    Q_PROPERTY(bool editable READ isEditable)
    Q_PROPERTY(Type type READ type)
    Q_PROPERTY(QString typeName READ typeName)

public:
    enum Type {
        UnknownType,
        DatabaseAttributeType
    };

    QString name() const;
    virtual void setName(const QString &name);

    QString displayName() const;
    void setDisplayName(const QString &name);

    bool isEditable() const;
    Type type() const;
    QString typeName() const;


    Attribute *createAttributeInstance(Row *row);

    static QString typeToName(Type type);

signals:
    void nameChanged();
    void displayNameChanged();

protected:
    friend class TableMetaData;
    friend class Table;

    static AttributeMetaData *createMetaDataInstance(const QSqlQuery &query, Table *table, TableMetaData *metaData);

    AttributeMetaData(const QSqlQuery &query, Table *table, TableMetaData *metaData);

    Table *m_table;
    QString m_name;
    QString m_displayName;
    Type m_type;
    bool m_isEditable;
};

} // namespace LBDatabase

#endif // LBDATABASE_ATTRIBUTEMETADATA_H
