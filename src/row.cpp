#include "row.h"

#include "column.h"
#include "database.h"
#include "table.h"

#include <QSqlQuery>
#include <QSqlRecord>

namespace LBDatabase {

/******************************************************************************
** RowPrivate
*/
class RowPrivate {
private:
    RowPrivate() :
        id(0),
        table(0)
    {}

    void initWith(const QSqlQuery &query);

    friend class Row;
    int id;
    Table *table;
    QList<QVariant> values;
};

void RowPrivate::initWith(const QSqlQuery &query)
{
    int idIndex = query.record().indexOf(QLatin1String("id"));
    Q_ASSERT_X(idIndex != -1, "RowPrivate::initWith", "The query has no field 'id'");

    id = query.value(idIndex).toInt();
    Q_ASSERT_X(id > 0, "RowPrivate::initWith", "The id of this row is not valid");

    values.reserve(query.record().count());
    for(int i = 0; i < query.record().count(); ++i) {
        values.append(query.value(i));
    }
}

/******************************************************************************
** Row
*/
/*!
  \class Row

  \brief The Row class represents a row in a Table.

  \ingroup lowlevel-database-classes

  There exists an instance of Row for each row in each table. You can use data()
  and setData() to query an manipulate the content of fields.

  You will seldomly instantiate this class on your own. Use the access and
  creation methods of Table instead (i.e. Table::appendRow() and Table::row()).

  \sa Table, Column
  */

/*!
  \fn void Row::dataChanged(int column, QVariant data)

  This signal is emitted whenever the content of the field in the Column at
  index \a column changes.

  The \a data parameter holds the new value.

  \sa data() and setData().
  */

/*!
  \var Row::d_ptr
  \internal
  */

/*!
  \internal

  Constructs a new row from the contents in the current record of \a query in
  the Table \a table.
  */
Row::Row(const QSqlQuery &query, Table *table) :
    QObject(table),
    d_ptr(new RowPrivate)
{
    Q_D(Row);
    d->table = table;
    d->initWith(query);
}

/*!
  Destroys the row.
  */
Row::~Row()
{
}

/*!
  Returns the ID of the row.
  */
int Row::id() const
{
    Q_D(const Row);
    return d->id;
}

/*!
  Returns the content stored in the database in this row in the Column at index
  \a column.
  */
QVariant Row::data(int column) const
{
    Q_D(const Row);
    return d->values.at(column);
}

/*!
  Returns the data stored in the row in the column named \a column or an invalid
  QVariant if this column does not exist in the table of the row.
  */
QVariant Row::data(const QString &column) const
{
    Q_D(const Row);

    Column *c = d->table->column(column);
    if(!c)
        return QVariant();

    return data(c->index());
}

/*!
  Sets the content stored in the database in this row in the Column at index
  \a column to \a data.
  Does nothing if no such column exists in the table of the row.
  */
void Row::setData(int column, const QVariant &data)
{
    Q_D(Row);
    if(column >= d->values.size() || d->values.at(column) == data)
        return;

    d->values.replace(column, data);
    QSqlQuery query(d->table->database()->sqlDatabase());
    query.exec(QLatin1String("UPDATE ")+d->table->name()+
               QLatin1String(" SET ")+d->table->column(column)->name()+QLatin1String(" = '")+data.toString()+
               QLatin1String("' WHERE id = '")+QString::number(d->id)+QLatin1String("'"));
    checkSqlError(query);
    query.finish();
    d->table->database()->setDirty(true);
    emit dataChanged(column, data);
}

/*!
  Sets the content stored in the database in this row in the Column with the
  name \a column to \a data.
  Does nothing if no such column exists in the table of the row.
  */
void Row::setData(const QString &column, const QVariant &data)
{
    Q_D(const Row);

    Column *c = d->table->column(column);
    if(!c)
        return;

    setData(c->index(), data);
}

/*!
  \internal

  Adds a column to the row.

  \sa Table::addColumn()
  */
void Row::addColumn(const QString &name, const QVariant &value)
{
    Q_UNUSED(name)
    Q_D(Row);
    d->values.append(value);
}

/*!
  \internal

  Removes a column from the row.

  \sa Table::removeColumn()
  */
void Row::removeColumn(int column)
{
    Q_D(Row);
    d->values.removeAt(column);
}

} // namespace LBDatabase
