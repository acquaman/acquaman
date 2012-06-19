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


#ifndef AMIONCHAMBERVIEW_H
#define AMIONCHAMBERVIEW_H

#include <QWidget>

#include <QLabel>
#include <QToolButton>
#include <QHBoxLayout>

#include "beamline/AMIonChamber.h"

/*!
  This view builds a standard view for AMIonChamber.  Although AMIonChamber is an abstract class, the interface it provides
  is all that is needed for this view to function.  Obviously, a subclass of AMIonChamber that can actually instantiate an
  object is required for this view to be used.
*/
class AMIonChamberView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Builds a view around the AMIonChamber \param chamber.
	/*!
	  This is a basic view.  It builds the a view that displays the description,
	  has the ability to increase or decrease the sensitivity, and then displays
	  either the counts, voltage, or a format string that displays the whether
	  the ion chamber is within the linear voltage range.
	  */
	explicit AMIonChamberView(AMIonChamber *chamber, QWidget *parent = 0);

	/// Sets the status string for the voltage being too high.
	void setHighStatus(const QString &name) { tooHigh_ = name; }
	/// Sets the status string for the voltage being too low.
	void setLowStatus(const QString &name) { tooLow_ = name; }
	/// Sets the within range status string for the voltage being within the acceptable range.
	void setWithinRangeStatus(const QString &name) { withinRange_ = name; }

	/// Returns a pointer to the chamber being viewed as an AMIonChamber.
	AMIonChamber *ionChamberAM() const { return chamber_; }

signals:

public slots:

protected slots:
	/// Handles the display of the output string based on the current voltage/counts output and displays the appropriate output based on the current view choice (counts, voltage, status string).
	virtual void onReadingsChanged();
	/// Builds a popup menu for switching view modes.
	virtual void onCustomContextMenuRequested(QPoint pos);

protected:
	/// Enum that holds the display state.
	enum Display { Counts, Voltage, Status };

	/// Pointer to the ion chamber being viewed.
	AMIonChamber *chamber_;

	/// Format string for being too high.
	QString tooHigh_;
	/// Format string for being too low.
	QString tooLow_;
	/// Format string for being within the correct range.
	QString withinRange_;

	/// State flag that holds what state the ion chamber is currently in.
	Display state_;

	/// The label that holds the name.
	QLabel *name_;
	/// The tool button for the minus button.
	QToolButton *minus_;
	/// The tool button for the plus button.
	QToolButton *plus_;
	/// The label that the format strings and the counts are displayed.
	QLabel *output_;

	/// The overall layout for the view.  This is so that subclasses can easily extend the view if they desired.
	QHBoxLayout *ionChamberViewLayout_;
};

#endif // AMIONCHAMBERVIEW_H
