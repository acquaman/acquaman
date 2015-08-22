#ifndef BIOXASSTRIPTOOL_H
#define BIOXASSTRIPTOOL_H

#include "ui/AMStripTool.h"
#include "beamline/AMPVControl.h"

class BioXASStripTool : public AMStripTool
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASStripTool(QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASStripTool();

protected:
	/// Storage ring current control.
	AMReadOnlyPVControl *ringCurrent_;
};

#endif // BIOXASSTRIPTOOL_H
