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


#ifndef VESPERSBEAMSELECTORVIEW_H
#define VESPERSBEAMSELECTORVIEW_H

#include <QWidget>

#include <QButtonGroup>
#include <QAbstractButton>
#include <QProgressBar>

#include "application/VESPERS/VESPERS.h"

/*!
  A view for the VESPERSBeamSelector model.  The view itself is simple with only four buttons and a progress bar that shows up to show the progress to the next beam.
  The current beam will be highlighted (in green for now) to show the current beam.  The progress bar will be hidden except when it is in use.
  */
class VESPERSBeamSelectorView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.  Takes a VESPERSBeamSelector as an argument.
 	virtual ~VESPERSBeamSelectorView();
	explicit VESPERSBeamSelectorView(QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Handles changing the color of the right button to show which beam your on.
	void onCurrentBeamChanged(VESPERS::Beam beam);

	/// Tells the model to move the given beam.
	void changeBeam(int id);

	/// Handles the clean up after changing beams.
	void onBeamChangeCompleted() { progressBar_->hide(); }


protected:
	/// Button group containing all the buttons to the different beams.
	QButtonGroup *beams_;
	/// Arbitrary button pointer to the current selected beam.
	QAbstractButton *currentBeam_;
	/// The progress bar.
	QProgressBar *progressBar_;
};

#endif // VESPERSBEAMSELECTORVIEW_H
