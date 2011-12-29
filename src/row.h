#ifndef LBDATABASE_ROW_H
#define LBDATABASE_ROW_H

#include <QObject>

#include <QSqlQuery>
#include <QHash>
#include <QMetaType>

namespace LBDatabase {

class Table;
class Attribute;
class DatabaseAttribute;

typedef QObject Calculator;

class Row : public QObject
{
    Q_OBJECT
public:
    const int &id() const;

    Table *table() const;

    /*!
      Returns all Attributes of the owner.
      */
    const QList<Attribute *> attributes() const;

    /*!
      Returns all DatabaseAttributes of the owner.
      */
    const QList<DatabaseAttribute *> databaseAttributes() const;

    /*!
      Returns the Attribute with the name \p name or 0, if no such Attribute exists for the owner.
      */
    Attribute *attribute(const QString &name) const;

    /*!
      Adds the DatabaseAttribute \p attribute to the owner.

      You must also call addAttribute(AttributeBase*) on \p attribute!
      */
    void addDatabaseAttribute(DatabaseAttribute *attribute);

    /*!
      Adds the Attribute \p attribute to the owner.
      */
    void addAttribute(Attribute *attribute);

    /*!
      Returns the calculator of the owner.
      */
    Calculator* calculator() const;

protected:
    friend class Table;

    explicit Row();
    explicit Row(const Row &other);
    explicit Row(int id, Table *parent);
    void initWith(int id, Table *parent);

    int m_id;
    QHash<QString, Attribute *> m_attributeByName;
    QList<DatabaseAttribute *> m_databaseAttributes;
    QList<Attribute *> m_attributes;
    Calculator* m_calculator;
    Table *m_table;
};

} // namespace LBDatabase

Q_DECLARE_METATYPE(LBDatabase::Row*)

#define LB_ROW(RowClassname) \
    public: \
    explicit RowClassname(); \
    explicit RowClassname(int id, LBDatabase::Table *parent = 0)

#define INHERITS
#define LB_REGISTER_ROW(Classname, SuperClassName) \
    Q_DECLARE_METATYPE(Classname) \
    LBDatabase::lbQRegisterMetaTypeHelper<Classname> _register_ ## Classname(QLatin1String("")); \
    Classname::Classname() : \
        SuperClassName() \
    {} \
    Classname::Classname(int id, LBDatabase::Table *parent) : \
        SuperClassName(id,parent) \
    {}

/* /    Q_DECLARE_METATYPE(Classname*) \
//    Classname::Classname(::LBDatabase::RowRegistrar* h) : SuperClassName(h) { initializeAttributes(); }  \
//    Classname::Classname(const Classname &other) : SuperClassName(other.m_id, other.m_table) { initializeAttributes(); } \
//    Classname::Classname(int id, ::LBDatabase::TableBase *table) : SuperClassName(id,table) { initializeAttributes(); } \
//    ::LBDatabase::lbQRegisterMetaTypeHelper<Classname> _register_ ## classname(QLatin1String(STRINGIZE(databaseName) ""));
*/

#endif // LBDATABASE_ROW_H
