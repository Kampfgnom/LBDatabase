#ifndef LBDATABASE_ROW_H
#define LBDATABASE_ROW_H

#include <QObject>

#include <QVariant>

class QSqlQuery;

namespace LBDatabase {

class Table;

class RowPrivate;
class Row : public QObject
{
    Q_OBJECT
public:
    ~Row();

    QVariant data(int column) const;
    void setData(int column, const QVariant &data);

Q_SIGNALS:
    void dataChanged(int column, QVariant data);

private:
    friend class TablePrivate;
    void addColumn(const QString &name, const QVariant &value);
    void removeColumn(int column);

    explicit Row(const QSqlQuery &query, Table *table);

    RowPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(Row)
};

} // namespace LBDatabase

#endif // LBDATABASE_ROW_H
