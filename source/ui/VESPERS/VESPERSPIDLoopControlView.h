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


#ifndef VESPERSPIDLOOPCONTROLVIEW_H
#define VESPERSPIDLOOPCONTROLVIEW_H

#include <QWidget>

#include <QTimer>
#include <QPushButton>

#include "beamline/VESPERS/VESPERSPIDLoopControl.h"

class VESPERSPIDLoopControlView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.  Requires a valid PID loop control.
	explicit VESPERSPIDLoopControlView(VESPERSPIDLoopControl *pid, QWidget *parent = 0);
	/// Destructor.
	virtual ~VESPERSPIDLoopControlView();

signals:

public slots:

protected slots:
	/// On visible changed.
	void onHiddenChanged(bool hidden);
	/// Toggle the button color.
	void toggleButtonColor();

protected:
	/// The pointer to the PID control.
	VESPERSPIDLoopControl *pid_;

	/// The timer.
	QTimer *timer_;
	/// Helper bool to know which color to put in the button.
	bool highlight_;
	/// The button.
	QPushButton *fix_;
};

#endif // VESPERSPIDLOOPCONTROLVIEW_H
