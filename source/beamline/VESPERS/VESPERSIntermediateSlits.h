#ifndef VESPERSINTERMEDIATESLITS_H
#define VESPERSINTERMEDIATESLITS_H

#include <QObject>

#include "beamline/AMProcessVariable.h"

/*!
  This class encapsulates the intermediate slits for the VESPERS beamline.  Currently, only the gap is changeable.  If it is desired, the center position will also be added
  to the class.  The slits that are controllable fromm this class are the X and Z slits.
  */
class VESPERSIntermediateSlits : public QObject
{
	Q_OBJECT
public:
	/// Constructor.
	explicit VESPERSIntermediateSlits(QObject *parent = 0);

	/// Returns the X slit gap.
	double gapX() const { return x_->getDouble(); }
	/// Returns the Z slit gap.
	double gapZ() const { return z_->getDouble(); }

signals:
	/// Notifier that the X slit gap has changed.  Passes the new gap size.
	void gapXChanged(double);
	/// Notifier that the Z slit gap has changed.  Passes the new gap size.
	void gapZChanged(double);

public slots:
	/// Sets the X slit gap.
	void setGapX(double gap) { x_->setValue(gap); }
	/// Sets the Z slit gap.
	void setGapZ(double gap) { z_->setValue(gap); }

protected:
	/// The X slit.
	AMProcessVariable *x_;
	/// The Z slit.
	AMProcessVariable *z_;
};

#endif // VESPERSINTERMEDIATESLITS_H
