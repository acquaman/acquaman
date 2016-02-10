#ifndef CLSJJSLIT_H
#define CLSJJSLIT_H

#include "beamline/AMSlit.h"

class CLSJJSlit : public AMSlit
{
    Q_OBJECT

public:
	/// Constructor.
	explicit CLSJJSlit(const QString &name, const QString &positiveBladeName, const QString &negativeBladeName, QObject *parent = 0);
	/// Destructor.
	virtual ~CLSJJSlit();
};

#endif // CLSJJSLIT_H
