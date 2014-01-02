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


#ifndef AM2DSCANCONFIGURATIONGENERALVIEW_H
#define AM2DSCANCONFIGURATIONGENERALVIEW_H

#include "AMDbObjectGeneralView.h"

class AM2DScanConfigurationGeneralView : public AMDbObjectGeneralView
{
Q_OBJECT
public:
 	virtual ~AM2DScanConfigurationGeneralView();
	Q_INVOKABLE AM2DScanConfigurationGeneralView(QWidget *parent = 0);

	virtual void createView(const QString &databaseName, const QString &tableName, int dbId);
	virtual bool canView(const QString &databaseName, const QString &tableName, int dbId);
};

#endif // AM2DSCANCONFIGURATIONGENERALVIEW_H
