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


#ifndef AMDATAMANAPPCONTROLLERFORACTIONS3_H
#define AMDATAMANAPPCONTROLLERFORACTIONS3_H

#include "application/AMDatamanAppController.h"

/// This version of AMDatamanAppController is suitable for apps using the 'actions3' actions framework
class AMDatamanAppControllerForActions3 : public AMDatamanAppController
{
    Q_OBJECT
public:
	explicit AMDatamanAppControllerForActions3(QObject *parent = 0);

	virtual ~AMDatamanAppControllerForActions3();

	/// Re-implemented to create the user database and actions database
	virtual bool startupCreateDatabases();
	/// Re-implemented to register common database classes, and the actions3 database objects.
	virtual bool startupRegisterDatabases();

signals:

public slots:

};

#endif // AMDATAMANAPPCONTROLLERFORACTIONS3_H
