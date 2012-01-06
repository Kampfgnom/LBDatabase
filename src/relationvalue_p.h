#ifndef LBDATABASERELATIONVALUE_P_H
#define LBDATABASERELATIONVALUE_P_H

#include <QObject>

namespace LBDatabase {

class Entity;
class Relation;
class RelationValue;

class RelationValuePrivate {
protected:
    friend class RelationPrivate;

    RelationValuePrivate() {}

    void init();
    void fetchValue();

    void addOtherEntity(Entity *entity);

    Entity *entity;
    Relation *relation;
    QList<Entity *> otherEntities;

    RelationValue * q_ptr;
    Q_DECLARE_PUBLIC(RelationValue)
};

} // namespace LBDatabase

#endif // LBDATABASERELATIONVALUE_P_H
