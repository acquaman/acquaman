/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMVALVECONTROL_H
#define AMVALVECONTROL_H

#include <QObject>

#include "beamline/AMProcessVariable.h"

/*!
	This is a bare bones control class.  It doesn't inherit AMControl right now because this is currently a class meant to be used while we
	still don't have a good way of handling controls of this type.  This will probably be replaced once a better distinction between analog
	and digital controls is established in AMControl.
  */
class AMValveControl : public QObject
{
	Q_OBJECT
public:
	/// Constructor.  Takes in the names of the PVs for reading, opening and closing.
	explicit AMValveControl(QString name, QString readPV, QString openPV, QString closePV, QObject *parent = 0);

	/// Returns the name of the Valve control.
	QString name() const { return name_; }

	/// Get the current status of the control. 0 is closed, 1 is between, and 2 is open.
	int state() const
	{
		switch(readPV_->getInt()){
		case 1:
			return 2;
		case 4:
			return 0;
		default:
			return 1;
		}
	}
	/// Same as state() but returns a QString. Passes Open, Between, or Closed.
	QString stateString() const
	{

		switch(readPV_->getInt()){
		case 1:
			return QString("Open");
		case 4:
			return QString("Closed");
		default:
			return QString("Between");
		}
	}

signals:
	/// Notifies that the status has changed and passes on the state.
	void stateChanged(int);

public slots:
	/// Open the valve.
	void openValve() { openPV_->setValue(1); }
	/// Close the valve.
	void closeValve() { closePV_->setValue(1); }
	/// Sets the name of the valve control.
	void setName(QString name) { name_ = name; }

protected:

	// The name.
	QString name_;

	// The PVs.
	AMProcessVariable *readPV_;
	AMProcessVariable *openPV_;
	AMProcessVariable *closePV_;
};

#endif // AMVALVECONTROL_H
