#include "entity.h"

namespace LBDatabase {

/******************************************************************************
** EntityPrivate
*/
class EntityPrivate {
    EntityPrivate() {}

    void init();

    Entity * q_ptr;
    Q_DECLARE_PUBLIC(Entity)
};

void EntityPrivate::init()
{
}

/******************************************************************************
** Entity
*/
Entity::Entity(QObject *parent) :
    QObject(parent),
    d_ptr(new EntityPrivate)
{
    Q_D(Entity);
    d->q_ptr = this;
    d->init();
}

Entity::~Entity()
{
    Q_D(Entity);
    delete d;
}

} // namespace LBDatabase
