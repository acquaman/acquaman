#ifndef AMLISTACTIONEDITOR3_H
#define AMLISTACTIONEDITOR3_H

#include <QFrame>
#include "actions3/AMListActionInfo3.h"

class AMListActionEditor3 : public QFrame
{
Q_OBJECT
public:
	Q_INVOKABLE AMListActionEditor3(AMListActionInfo3 *info, QWidget *parent = 0);

protected:
	AMListActionInfo3 *info_;
};

#endif // AMLISTACTIONEDITOR3_H
