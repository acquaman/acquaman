#ifndef AMLISTACTIONINFO3_H
#define AMLISTACTIONINFO3_H

#include "actions3/AMActionInfo3.h"

class AMListActionInfo3 : public AMActionInfo3
{
Q_OBJECT

public:
	Q_INVOKABLE AMListActionInfo3(QObject *parent = 0);

	AMListActionInfo3(const AMListActionInfo3 &other);

	virtual AMActionInfo3* createCopy() const;

	virtual QString typeDescription() const;
};

#endif // AMLISTACTIONINFO3_H
