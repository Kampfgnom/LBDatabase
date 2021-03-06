#ifndef LBDATABASE_TABLE_H
#define LBDATABASE_TABLE_H

#include <QAbstractTableModel>

namespace LBDatabase {

class Column;
class Database;
class Row;

class TablePrivate;
class Table : public QAbstractTableModel
{
    Q_OBJECT

public:
    ~Table();

    QString name() const;
    Database *database() const;

    Column *column(int column) const;
    QList<Column *> columns() const;
    QStringList columnNames() const;

    void changeColumnName(const QString &name, const QString &newName);
    Column *addColumn(const QString &name, const QString &sqlType, const QVariant &defaultValue = QVariant());
    void removeColumn(const QString &name);

    Row *appendRow();
    Row *row(int id) const;
    QList<Row *> rows() const;

    // QAbstractTableModel
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;

protected:
    friend class DatabasePrivate;

    void emitHeaderDataChanged(Qt::Orientation orientation, int first, int last);

    Table(const QString &name, Database *database);

    TablePrivate * const d_ptr;
    Q_DECLARE_PRIVATE(Table)
    Q_DISABLE_COPY(Table)
};

} // namespace LBDatabase

Q_DECLARE_METATYPE(LBDatabase::Table *)

#endif // LBDATABASE_TABLE_H
