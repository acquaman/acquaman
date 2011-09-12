/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef VESPERSINTERMEDIATESLITSVIEW_H
#define VESPERSINTERMEDIATESLITSVIEW_H

#include <QWidget>

#include <QDoubleSpinBox>

#include "beamline/VESPERS/VESPERSIntermediateSlits.h"

class VESPERSIntermediateSlitsView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.  Takes in the VESPERSIntermediateSlit as an argument.
	explicit VESPERSIntermediateSlitsView(VESPERSIntermediateSlits *slits, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// If a new value for the X slit gap, passes it down to the slits model.
	void setXGap() { if (xSlit_->value() != slits_->gapX()) slits_->setGapX(xSlit_->value()); }
	/// If a new value for the Z slit gap, passes it down to the slits model.
	void setZGap() { if (zSlit_->value() != slits_->gapZ()) slits_->setGapZ(zSlit_->value()); }

protected:
	/// Spin box handling the X slit gap.
	QDoubleSpinBox *xSlit_;
	/// Spin box handling the Z slit gap.
	QDoubleSpinBox *zSlit_;

	/// Pointer to the slits.
	VESPERSIntermediateSlits *slits_;
};

#endif // VESPERSINTERMEDIATESLITSVIEW_H
