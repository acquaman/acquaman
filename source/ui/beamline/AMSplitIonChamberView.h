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


#ifndef AMSPLITIONCHAMBERVIEW_H
#define AMSPLITIONCHAMBERVIEW_H

#include "ui/beamline/AMIonChamberView.h"
#include "beamline/AMSplitIonChamber.h"

/*!
  This view builds a standard view for AMIonChamber.  Although AMIonChamber is an abstract class, the interface it provides
  is all that is needed for this view to function.  Obviously, a subclass of AMIonChamber that can actually instantiate an
  object is required for this view to be used.
*/
class AMSplitIonChamberView : public AMIonChamberView
{
	Q_OBJECT

public:
	/// Constructor.  Builds a view around the AMSplitIonChamber \param chamber.
	/*!
	  This is a basic view.  It builds the a view that displays the description,
	  has the ability to increase or decrease the sensitivity, and then displays
	  either the counts, voltage, or a format string that displays the whether
	  the ion chamber is within the linear voltage range.
	  */
	AMSplitIonChamberView(AMSplitIonChamber *chamber, QWidget *parent = 0);

	/// Returns the pointer viewed by this ion chamber as a split ion chamber.  Returns 0 if not valid since it needs to be casted.
	AMSplitIonChamber *splitIonChamberAM() const { return qobject_cast<AMSplitIonChamber *>(chamber_); }

protected slots:
	/// Handles the display of the output string based on the current voltage/counts output and displays the appropriate output based on the current view choice (counts, voltage, status string).
	virtual void onReadingsChanged();
	/// Handles showing either the locked sensitivity or each ion chamber separately.  Both views are still just plus and minus buttons.
	virtual void onModeViewChanged();
	/// Builds a popup menu for switching view modes.
	virtual void onCustomContextMenuRequested(QPoint pos);

protected:
	/// The tool button for the minus button for one side of the ion chamber (A).
	QToolButton *minusA_;
	/// The tool button for the plus button for one side of the ion chamber (A).
	QToolButton *plusA_;
	/// The tool button for the minus button for the other side of the ion chamber (B).
	QToolButton *minusB_;
	/// The tool button for the plus button for the other side of the ion chamber (B).
	QToolButton *plusB_;

	/// Flag that holds whether the split ion chamber has individual control over the sensitivity or if the they are locked together.
	bool isLocked_;
};

#endif // AMSPLITIONCHAMBERVIEW_H
