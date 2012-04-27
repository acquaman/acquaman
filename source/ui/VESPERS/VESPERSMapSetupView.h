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


#ifndef VESPERSMAPSETUPVIEW_H
#define VESPERSMAPSETUPVIEW_H

#include <QWidget>
#include <QRadioButton>

#include "util/VESPERS/VESPERSMapSetup.h"

/*!
	This widget sets up the arguments to boot up the NDMapper software without users having to go through the terminal application.
  */
class VESPERSMapSetupView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.  Builds the widget and builds a view around the VESPERSMapSetup that is passed to it.
	explicit VESPERSMapSetupView(VESPERSMapSetup *mapSetup, QWidget *parent = 0);

protected slots:
	/// Handles changes to the dimension.
	void onDimensionChanged(bool is2D) { is2D ? dim2D_->setChecked(true) : dim3D_->setChecked(true); }
	/// Handles changes to the detector.
	void onDetectorChanged(bool isSingleElement) { isSingleElement ? vortex1El_->setChecked(true) : vortex4El_->setChecked(true); }
	/// Handles setting the dimension of the configuration.  True = 2D, false = 3D.
	void setDimension(bool is2D) { is2D ? mapSetup_->set2D() : mapSetup_->set3D(); }
	/// Handles setting the detector.  True = single element, false = four element.
	void setDetector(bool isSingleElement) { isSingleElement ? mapSetup_->useSingleElement() : mapSetup_->useFourElement(); }

protected:
	/// Pointer to the map setup model this view is showing.
	VESPERSMapSetup *mapSetup_;

	/// Radio button for the 2D map.
	QRadioButton *dim2D_;
	/// Radio button for the 3D map.
	QRadioButton *dim3D_;
	/// Radio button for the single element vortex.
	QRadioButton *vortex1El_;
	/// Radio button for the four element vortex.
	QRadioButton *vortex4El_;
};

#endif // VESPERSMAPSETUPVIEW_H
