#ifndef AMCONSTDBOBJECT_H
#define AMCONSTDBOBJECT_H

#include "dataman/database/AMDbObject.h"

class AMConstDbObject : public QObject
{
Q_OBJECT
public:
	/// Constructor, takes an underlying AMDbObject*
	AMConstDbObject(const AMDbObject *object, QObject *parent = 0);

	/// Retrieves the object, you will need to qobject_cast it to the type you desire
	const AMDbObject* object() const;

public slots:
	/// Sets the object
	void setObject(const AMDbObject *object);

protected:
	/// The object being stored
	const AMDbObject *object_;
};

Q_DECLARE_METATYPE(AMConstDbObject*)

#endif // AMCONSTDBOBJECT_H
