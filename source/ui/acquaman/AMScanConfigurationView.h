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


#ifndef AMSCANCONFIGURATIONVIEW_H
#define AMSCANCONFIGURATIONVIEW_H

#include <QWidget>


class AMScanConfiguration;

/// If you want your scan configuration widget to easily be able to start scans, you should inherit from AMScanConfigurationView, and install it within an AMScanConfigurationHolder.
class AMScanConfigurationView : public QWidget {
	Q_OBJECT

public:
	/// Default constructor
 	virtual ~AMScanConfigurationView(){}
	explicit AMScanConfigurationView(QWidget* parent = 0) : QWidget(parent) {}

	/// This function must return a pointer to the scan configuration, that is being configure within your view.
	/*! Any functions that use this will make a copy of the configuration immediately, so you don't need to worry about making changes to the pointed-to object.*/
	virtual const AMScanConfiguration* configuration() const = 0;

protected:
	/// Helper method that takes a time in seconds and returns a string of d:h:m:s.
	QString convertTimeToString(double time)
	{
		QString timeString;

		int days = int(time/3600.0/24.0);

		if (days > 0){

			time -= days*3600.0*24;
			timeString += QString::number(days) + "d:";
		}

		int hours = int(time/3600.0);

		if (hours > 0){

			time -= hours*3600;
			timeString += QString::number(hours) + "h:";
		}

		int minutes = int(time/60.0);

		if (minutes > 0){

			time -= minutes*60;
			timeString += QString::number(minutes) + "m:";
		}

		int seconds = ((int)time)%60;
		timeString += QString::number(seconds) + "s";

		return timeString;
	}

};


#endif // AMSCANCONFIGURATIONVIEW_H
