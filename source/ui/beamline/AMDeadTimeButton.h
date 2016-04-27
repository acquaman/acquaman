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


#ifndef AMDEADTIMEBUTTON_H
#define AMDEADTIMEBUTTON_H

#include "ui/AMToolButton.h"
#include "dataman/datasource/AMDataSource.h"

/// This class takes a data source (expected to be the dead time) and changes the color based on the current value and the chosen reference positions.  The expected behaviour is anything below the "good" setpoint is green, anything above the "bad" setpoint is red, and everything else is yellow.
class AMDeadTimeButton : public AMToolButton
{
	Q_OBJECT

public:
	/// Constructor.  Makes a default button with no changing other than looking disabled.
	AMDeadTimeButton(QWidget *parent = 0);
	/// Constructor.  Takes two data sources (the data source is assumed to have rank 0), one for the input counts and one for the output counts, the good reference point, and the bad reference point.
	AMDeadTimeButton(AMDataSource *inputCountSource, AMDataSource *outputCountSource, double goodReferencePoint, double badReferencePoint, bool displayPercent = true, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMDeadTimeButton();

	/// Returns the input count source.
	AMDataSource* inputCountSource() const { return inputCountSource_; }
	/// Returns the output count source.
	AMDataSource* outputCountSource() const { return outputCountSource_; }
	/// Returns the good reference point currently used by this button.
	double goodReferencePoint() const { return goodReferencePoint_; }
	/// Returns the bad reference point currently used by this button.
	double badReferencecPoint() const { return badReferencePoint_; }

public slots:
	/// Sets a new data source for the dead time (the data source is assumed to have rank 0).  Sources can be 0/null pointers.
	void setDeadTimeSources(AMDataSource *inputCountSource, AMDataSource *outputCountSource);
	/// Sets the good reference point.
	void setGoodReferencePoint(double newReference);
	/// Sets the bad reference point.
	void setBadReferencePoint(double newReference);
	/// Sets the flag indicating whether to display dead time as percent (or counts).
	void setDisplayAsPercent(bool showPercent);

protected slots:
	/// Updates the color state.
	void updateColorState();
	/// Method that updates the status text of the button and class update.
	void updateToolTip();

protected:
	/// Helper method that returns whether there are valid data sources or not.
	bool hasDeadTimeSources() const;
	/// Helper method that returns whether there is a valid ICR data source.
	bool hasICRDataSource() const;

	/// The data source that holds the input counts.
	AMDataSource *inputCountSource_;
	/// The data source that holds the output counts.
	AMDataSource *outputCountSource_;
	/// The good reference point.
	double goodReferencePoint_;
	/// The bad reference point.
	double badReferencePoint_;
	/// The flag indicating whether to display dead time as percent (or counts).
	bool displayPercent_;
};

#endif // AMDEADTIMEBUTTON_H
