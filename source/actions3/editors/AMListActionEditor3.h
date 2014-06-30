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


#ifndef AMLISTACTIONEDITOR3_H
#define AMLISTACTIONEDITOR3_H

#include <QFrame>
#include "actions3/AMListActionInfo3.h"

class AMListActionEditor3 : public QFrame
{
Q_OBJECT
public:
 	virtual ~AMListActionEditor3();
	Q_INVOKABLE AMListActionEditor3(AMListActionInfo3 *info, QWidget *parent = 0);

protected:
	AMListActionInfo3 *info_;
};

#endif // AMLISTACTIONEDITOR3_H
