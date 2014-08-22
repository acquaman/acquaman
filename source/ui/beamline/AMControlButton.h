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


#ifndef AMCONTROLBUTTON_H
#define AMCONTROLBUTTON_H

#include <QToolButton>
class AMControl;


class AMControlButton : public QToolButton
{
Q_OBJECT
public:
 	virtual ~AMControlButton();
	AMControlButton(AMControl *control, QWidget *parent = 0);

	int downValue() const;
	int upValue() const;

public slots:
	void setDownValue(double downValue);
	void setUpValue(double upValue);

	void overrideText(const QString& text);
	void setCheckable(bool);

protected slots:
	void onConnected(bool connected);

	void onValueChanged(double newVal);
	void onClicked();
	void onToggled(bool toggled);

protected:
	AMControl *control_;
	double downValue_;
	double upValue_;
	bool programaticToggle_;

	/// Bool handling whether the detector was connected.
	bool wasConnected_;
};


#endif // AMCONTROLBUTTON_H
