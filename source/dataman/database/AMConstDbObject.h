#ifndef AMCONSTDBOBJECT_H
#define AMCONSTDBOBJECT_H

#include "dataman/database/AMDbObject.h"

class AMConstDbObject : public QObject
{
Q_OBJECT
public:
	AMConstDbObject(const AMDbObject *object, QObject *parent = 0);

	const AMDbObject* object() const;

public slots:
	void setObject(const AMDbObject *object);

protected:
	const AMDbObject *object_;
};

Q_DECLARE_METATYPE(AMConstDbObject*)

#endif // AMCONSTDBOBJECT_H
