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


#ifndef VESPERSBENDINGMIRRORSVIEW_H
#define VESPERSBENDINGMIRRORSVIEW_H

#include <QWidget>

#include "beamline/AMPVControl.h"

class QLabel;
class QDoubleSpinBox;
class QLineEdit;

/// This class handles the layout for an individual line in (and conversely, motor).  It has the status indicator, the position, feedback, and stop button.
class VESPERSBendingMirrorsElementView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Takes a control and connects all the aspects of it to the widget levels.
 	virtual ~VESPERSBendingMirrorsElementView();
	explicit VESPERSBendingMirrorsElementView(AMControl *control, QWidget *parent = 0);

protected slots:
	/// Helper slot that handles setting all the initial values.
	void onControlConnected(bool isConnected);
	/// Helper slot that changes the pixmap for the status label.
	void onStatusChanged(bool moving);
	/// Helper slot that updates the setpoint line edit if the setpoint changes.
	void onSetpointChanged(double val);
	/// Helper slot that tells the control to move based on the setpoint.
	void onMoveRequested();
	/// Helper slot that performs a negative jog movement from the current position.
	void onJogMinus();
	/// Helper slot that performs a positive jog movement from the current position.
	void onJogPlus();
	/// Helper slot that sets the feedback label.
	void onFeedbackChanged();

protected:
	/// Pointer to the control.
	AMControl *control_;
	/// Pointer to the status label.
	QLabel *status_;
	/// Pointer to the line edit holding setpoint.
	QLineEdit *setpoint_;
	/// Pointer to the spin box holding the jog size.
	QDoubleSpinBox *jog_;
	/// Pointer to the label holding the feedback.
	QLabel *feedback_;
};

/// This is a standalone class that encapsulates the eight motors for moving the M3 and M4 motors.
class VESPERSBendingMirrorsView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Builds all the motors and the layout.
 	virtual ~VESPERSBendingMirrorsView();
	explicit VESPERSBendingMirrorsView(QWidget *parent = 0);
};

#endif // VESPERSBENDINGMIRRORSVIEW_H
