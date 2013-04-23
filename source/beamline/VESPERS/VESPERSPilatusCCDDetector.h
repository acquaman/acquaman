#ifndef VESPERSPILATUSCCDDETECTOR_H
#define VESPERSPILATUSCCDDETECTOR_H

#include "beamline/VESPERS/VESPERSCCDDetector.h"

/*!
	This class encapsulates the Pilatus detector on the VESPERS beamline.  For now it will only
	extend the base class.  However, since this detector has many features that we will likely
	use in the future, this class will likely be extended a lot more.
  */
class VESPERSPilatusCCDDetector : public VESPERSCCDDetector
{
	Q_OBJECT

public:
	/// Constructor.
	VESPERSPilatusCCDDetector(const QString &name, const QString &description, QObject *parent = 0);

protected:
	/// Currently a castrated function because the Pilatus comes with good viewing software.
	virtual void loadImageFromFileImplementation(const QString &filename) { Q_UNUSED(filename); }
};

#endif // VESPERSPILATUSCCDDETECTOR_H
