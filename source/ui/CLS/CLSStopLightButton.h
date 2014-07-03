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


#ifndef CLSSTOPLIGHTBUTTON_H
#define CLSSTOPLIGHTBUTTON_H

#include <QPushButton>
#include <QRadialGradient>

#include "beamline/CLS/CLSBiStateControl.h"

/*!
	This class offers a visualization to the CLSBiStateControl.  That control can return values of either Open, Closed, or Between; fitting nicely with the concept of a stop light.
	The colours are handed out in the following manner:

		Green = Open
		Yellow = Between
		Red = Closed

	This class does not create a control, it simply provides the visuals for an existing one.
  */
class CLSStopLightButton : public QPushButton
{
	Q_OBJECT
public:
	/// Constructor.  Requires the CLSBiStateControl.
 	virtual ~CLSStopLightButton();
	CLSStopLightButton(CLSBiStateControl *control, QWidget *parent = 0);

protected slots:
	/// Handles the look of the button if the connectivity changes.  It defaults to a "flat" look when it is disconnected.
	void onConnectedChanged(bool connected) { setFlat(!connected); }

protected:
	/// Re-implemented paint function.
	virtual void paintEvent(QPaintEvent *e);
	/// Re-implemented mouse release event that does not emit the clicked signal if the button is flat.
	virtual void mouseReleaseEvent(QMouseEvent *e);


	/// Control that this stop light represents.  This is a CLSBiStateControl rather than an AMControl because of the specific nature of the button.
	CLSBiStateControl *control_;
};

#endif // CLSSTOPLIGHTBUTTON_H
