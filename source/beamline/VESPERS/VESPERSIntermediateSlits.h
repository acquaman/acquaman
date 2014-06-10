/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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
 	virtual ~VESPERSIntermediateSlits();
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
