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


#ifndef AMROTATIONALOFFSET_H
#define AMROTATIONALOFFSET_H

#include <QVector3D>
#include "dataman/database/AMDbObject.h"

class AMRotationalOffset : public AMDbObject
{
	Q_OBJECT
	Q_PROPERTY(QVector3D rotationalOffset READ rotationalOffset WRITE setRotationalOffset)

public:
	AMRotationalOffset(QObject *parent = 0);
	AMRotationalOffset(double x, double y, double z, QObject *parent = 0);
	AMRotationalOffset(QVector3D rotationalOffset, QObject *parent = 0);
	QVector3D rotationalOffset();
	void setRotationalOffset(QVector3D rotationalOffset);
	void setRotationalOffset(double x, double y, double z);

private:
	QVector3D rotationalOffset_;
};

#endif // AMROTATIONALOFFSET_H
