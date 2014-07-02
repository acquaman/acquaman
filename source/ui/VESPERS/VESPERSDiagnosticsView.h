/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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

#ifndef VESPERSDIAGNOSTICSVIEW_H
#define VESPERSDIAGNOSTICSVIEW_H

#include <QWidget>
#include <QLabel>

#include "beamline/AMControlSet.h"

/// This class is simply an element for the diagnostics view.  It handles the connection to the individual element.
class VESPERSDiagnosticsViewElement : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.
	/*!
	  \param control is the AMControl that this element is watching.
	  \param name is the specific name this element should be using.
	  \param useValueAndUnits determines whether the view should show the value and units or a predetermined phrase of "GOOD" or "BAD".
	  */
 	virtual ~VESPERSDiagnosticsViewElement();
	explicit VESPERSDiagnosticsViewElement(AMControl *control, QString name, bool useValueAndUnits, QWidget *parent = 0);

protected slots:
	/// Handles updating the label.
	void onValueChanged(double val);

protected:
	/// Pointer to the control.
	AMControl *control_;
	/// Value label.
	QLabel *value_;
	/// Bool holding whether the value and units should be displayed, or a predefined phrase.
	bool useValueAndUnits_;
};

/*!
  This class is a customized general view class for diagnostics on the VESPERS beamline.  It takes in a control set
  and offset and prints out the pertinent information of the beamline in a quick manner.  The purpose is to get a
  quick look at the diagnostics of the beamline without resorting to looking at an EDM screen.
  */
class VESPERSDiagnosticsView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.
	/*!
	  \param set is the control set that is going to be viewed.
	  \param offset is the number of extra words in front of the name of the control.
	  \param useValueAndUnits determines whether the view should show the value and units or a predetermined phrase of "GOOD" or "BAD".
	  */
	explicit VESPERSDiagnosticsView(AMControlSet *set, int offset, bool useValueAndUnits, QWidget *parent = 0);

signals:

public slots:

};

#endif // VESPERSDIAGNOSTICSVIEW_H
