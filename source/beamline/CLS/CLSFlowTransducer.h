#ifndef CLSFLOWTRANSDUCER_H
#define CLSFLOWTRANSDUCER_H

#include "beamline/CLS/CLSBeamlineControl.h"

class CLSFlowTransducer : public CLSBeamlineControl
{
	Q_OBJECT

public:
	/// Constructor.
	CLSFlowTransducer(const QString &name, const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~CLSFlowTransducer();
};

#endif // CLSFLOWTRANSDUCER_H
