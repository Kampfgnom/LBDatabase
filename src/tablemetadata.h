#ifndef LBDATABASE_TABLEMETADATA_H
#define LBDATABASE_TABLEMETADATA_H

#include <QAbstractTableModel>

#include <QStringList>

namespace LBDatabase {

class AttributeMetaData;
class DatabaseAttributeMetaData;
class Table;

class TableMetaData : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit TableMetaData(Table *parent = 0);
    
    AttributeMetaData *attributeMetaData(const QString &name);
    QList<AttributeMetaData *> attributeMetaDatas();
    QList<DatabaseAttributeMetaData *> databaseAttributeMetaDatas();
    int attributeCount() const;

    void addAttribute();

    AttributeMetaData *attributeMetaDataByModelIndex(const QModelIndex &index);

    // QAbstractTableModel
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;

private slots:
    void attributeMetaDataNameChanged();
    void attributeMetaDataDisplayNameChanged();

private:
    friend class AttributeMetaData;
    friend class DatabaseAttributeMetaData;
    friend class Table;

    Table *m_table;
    QHash<QString, AttributeMetaData *> m_attributeMetaDataByName;
    QList<AttributeMetaData *> m_attributeMetaDatas;
    QList<DatabaseAttributeMetaData *> m_databaseAttributeMetaDatas;
};

} // namespace LBDatabase

#endif // LBDATABASE_TABLEMETADATA_H
