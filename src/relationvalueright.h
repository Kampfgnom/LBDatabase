#ifndef LBDATABASE_RELATIONVALUERIGHT_H
#define LBDATABASE_RELATIONVALUERIGHT_H

#include "relationvalue.h"

namespace LBDatabase {

class Relation;
class Entity;

class RelationValueRightPrivate;
class RelationValueRight : public RelationValue
{
    Q_OBJECT
public:
    ~RelationValueRight();

    QVariant data(int role = Qt::DisplayRole) const;
    bool setData(const QVariant &data);

private:
    friend class RelationPrivate;
    explicit RelationValueRight(Relation *relation, Entity *parent);

    void fetchValue();

    Q_DECLARE_PRIVATE(RelationValueRight)
    Q_DISABLE_COPY(RelationValueRight)
};

} // namespace LBDatabase

#endif // LBDATABASE_RELATIONVALUERIGHT_H
