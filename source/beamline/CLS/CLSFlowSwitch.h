#ifndef CLSFLOWSWITCH_H
#define CLSFLOWSWITCH_H

#include <QObject>

#include "beamline/AMBeamlineControl.h"

class CLSFlowSwitch : public AMBeamlineControl
{
	Q_OBJECT

public:
	/// An enumeration of the possible status states.
	enum Status { Bad = 0, Good = 1 };

	/// Constructor.
	CLSFlowSwitch(const QString &name, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~CLSFlowSwitch();
};

#endif // CLSFLOWSWITCH_H
