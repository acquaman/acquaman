#ifndef CLSFLOWTRANSDUCER_H
#define CLSFLOWTRANSDUCER_H

#include "beamline/AMBeamlineControl.h"

class CLSFlowTransducer : public AMBeamlineControl
{
	Q_OBJECT

public:
	/// An enumeration of the possible status states.
	enum Status { Bad = 0, Good = 1 };

	/// Constructor.
	CLSFlowTransducer(const QString &name, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~CLSFlowTransducer();
};

#endif // CLSFLOWTRANSDUCER_H
