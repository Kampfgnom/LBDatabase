#ifndef LBDATABASE_DATABASE_H
#define LBDATABASE_DATABASE_H

#include <QHash>
#include <QMap>
#include <QObject>
#include <QFile>
#include <QSqlDatabase>
#include <QList>
#include <QMetaType>

namespace LBDatabase {

class Table;

class Database : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QSqlDatabase sqlDatabase READ sqlDatabase)

public:

    static QList<Database*> instances();
    static Database *instance(const QString &name, const QString &fileName);
    static Database *instance(const QString &fileName);

    ~Database();

    QString name() const;
    QSqlDatabase sqlDatabase() const;
    QString fileName() const;

    Table *table(const QString &name) const;
    QList<Table *> tables() const;

private:
    explicit Database(const QString &name, const QString &fileName, QObject *parent);

    void initialize();

    void createTableInstances();
    Table *createTableInstace(const QString &name);

    static QHash<QString, Database*> s_instances;

    QString m_name;
    QFile m_file;
    QMap<QString, Table*> m_tables;

    Q_DISABLE_COPY(Database)
};

} // namespace LBDatabase

Q_DECLARE_METATYPE(LBDatabase::Database*)

#endif // LBDATABASE_DATABASE_H
