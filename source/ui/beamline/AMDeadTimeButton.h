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

class AMControl;

/// This class takes a data source (expected to be the dead time) and changes the color based on the current value and the chosen reference positions.  The expected behaviour is anything below the "good" setpoint is green, anything above the "bad" setpoint is red, and everything else is yellow.
class AMDeadTimeButton : public AMToolButton
{
	Q_OBJECT

public:
	/// Enumeration of different ways of displaying counts information.
	enum CountsMode { None = 0, Percent = 1, Counts = 2, CountRate = 3 };

	/// Constructor.  Makes a default button with no changing other than looking disabled.
	AMDeadTimeButton(QWidget *parent = 0);
	/// Constructor.  Takes two data sources (the data source is assumed to have rank 0), one for the input counts and one for the output counts, the good reference point, and the bad reference point.
	AMDeadTimeButton(AMDataSource *inputCountSource, AMDataSource *outputCountSource, double goodReferencePoint, double badReferencePoint, CountsMode CountsMode = Percent, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMDeadTimeButton();

	/// Returns true if this button can display counts information as a percentage.
	bool canDisplayPercentage() const;
	/// Returns true if this button can display counts information.
	bool canDisplayCounts() const;
	/// Returns true if this button can display counts information as a count rate.
	bool canDisplayCountRate() const;

	/// Returns the input count source.
	AMDataSource* inputCountSource() const { return inputCountSource_; }
	/// Returns the output count source.
	AMDataSource* outputCountSource() const { return outputCountSource_; }
	/// Returns the good reference point currently used by this button.
	double goodReferencePoint() const { return goodReferencePoint_; }
	/// Returns the bad reference point currently used by this button.
	double badReferencePoint() const { return badReferencePoint_; }
	/// Returns the display mode.
	CountsMode countsMode() const { return countsMode_; }
	/// Returns the acquire time control.
	AMControl* acquireTimeControl() const { return acquireTimeControl_; }

public slots:
	/// Sets a new data source for the dead time (the data source is assumed to have rank 0).  Sources can be 0/null pointers.
	void setDeadTimeSources(AMDataSource *inputCountSource, AMDataSource *outputCountSource);
	/// Sets the good reference point.
	void setGoodReferencePoint(double newReference);
	/// Sets the bad reference point.
	void setBadReferencePoint(double newReference);
	/// Sets the display mode.
	void setCountsMode(CountsMode newMode);
	/// Sets the acquire time control.
	void setAcquireTimeControl(AMControl *newControl);

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
	/// Helper method that returns whether there is a valid acquisition time.
	bool hasAcquireTime() const;

	/// Returns the counts as a percentage. Returns 0 if either the input or the output count source are invalid.
	double getPercent() const;
	/// Returns the counts. Returns 0 if the input count source is invalid.
	double getCounts() const;
	/// Returns the count rate. Returns 0 if the input count source or the time control are invalid.
	double getCountRate() const;
	/// Returns the acquire time. Returns 0 if the acquire time control is invalid.
	double getAcquireTime() const;

protected:
	/// The data source that holds the input counts.
	AMDataSource *inputCountSource_;
	/// The data source that holds the output counts.
	AMDataSource *outputCountSource_;
	/// The good reference point.
	double goodReferencePoint_;
	/// The bad reference point.
	double badReferencePoint_;
	/// The counts display mode.
	CountsMode countsMode_;
	/// The acquire time control.
	AMControl *acquireTimeControl_;
};

#endif // AMDEADTIMEBUTTON_H
