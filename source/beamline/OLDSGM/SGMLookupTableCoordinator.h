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


#ifndef SGMLOOKUPTABLECOORDINATOR_H
#define SGMLOOKUPTABLECOORDINATOR_H

#include "beamline/AMControlSet.h"
#include <QMap>

class SGMLookupTableCoordinator : public QObject
{
Q_OBJECT
public:
 	virtual ~SGMLookupTableCoordinator();
	SGMLookupTableCoordinator(const QString &inputControlName, const QString &outputControlName, const QString &linkageControlName, const QString &loadLookupTableControlName, QObject *parent = 0);

protected slots:
	void onInputControlChanged(double inputValue);
	/*
	void onInputControlChanged(double inputValue);
	void onLinkageControlChanged(double linkageValue);
	void onLoadLookupTableControlChanged(double loadLookupValue);

	*/
	void onAllControlsConnected(bool connected);

protected:
	/*
	void internalDoConversion();
	void internalLoadLookupTable();
	*/

protected:
	/*
	AMControl *inputControl_;
	AMControl *outputControl_;
	AMControl *linkageControl_;
	AMControl *loadLookupTableControl_;
	*/
	AMControl *inputControl_;
	AMControl *outputControl_;

	AMControlSet *allControls_;
	bool connectedOnce_;

	/*
	bool linkageActive_;
	QString lookupTableFullFileName_;

	QMap<double, double> lookupTable_;
	*/
};

#endif // SGMLOOKUPTABLECOORDINATOR_H
