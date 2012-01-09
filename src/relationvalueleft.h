#ifndef LBDATABASE_RELATIONVALUELEFT_H
#define LBDATABASE_RELATIONVALUELEFT_H

#include "relationvalue.h"

namespace LBDatabase {

class Entity;
class Relation;
class RelationValueRight;

class RelationValueLeftPrivate;
class RelationValueLeft : public RelationValue
{
    Q_OBJECT
public:
    ~RelationValueLeft();

    QVariant data(int role = Qt::DisplayRole) const;

private:
    friend class RelationPrivate;
    friend class RelationValueRightPrivate;

    explicit RelationValueLeft(Relation *relation, Entity *parent);
    virtual bool setData(const QVariant &data);

    void fetchValue();
    void setRightValue(RelationValueRight *right);

    Q_DECLARE_PRIVATE(RelationValueLeft)
    Q_DISABLE_COPY(RelationValueLeft)
};

} // namespace LBDatabase

#endif // LBDATABASE_RELATIONVALUELEFT_H
