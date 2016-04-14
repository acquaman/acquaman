#ifndef CLSFLOWSWITCH_H
#define CLSFLOWSWITCH_H

#include <QObject>

#include "beamline/CLS/CLSBeamlineControl.h"

class CLSFlowSwitch : public CLSBeamlineControl
{
	Q_OBJECT

public:
	/// Constructor.
	CLSFlowSwitch(const QString &name, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~CLSFlowSwitch();
};

#endif // CLSFLOWSWITCH_H
