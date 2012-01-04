#ifndef LBDATABASE_ENTITY_H
#define LBDATABASE_ENTITY_H

#include <QObject>

namespace LBDatabase {

class EntityPrivate;
class Entity : public QObject
{
    Q_OBJECT
public:
    explicit Entity(QObject *parent = 0);
    ~Entity();
    
private:
    EntityPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(Entity)
};

} // namespace LBDatabase

#endif // LBDATABASE_ENTITY_H
