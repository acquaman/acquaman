/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#ifndef AMBEAMLINECONTROLACTION_H
#define AMBEAMLINECONTROLACTION_H

#include "AMControl.h"
#include "AMBeamlineActionItem.h"

class AMBeamlineControlAction : public AMBeamlineActionItem
{
Q_OBJECT
public:
	AMBeamlineControlAction(AMControl *control, QObject *parent = 0);

	virtual QString type() const;

public slots:
	virtual void cleanup() {}

protected:
	AMControl *control_;

private:
	QString type_;
};



#endif // AMBEAMLINECONTROLACTION_H
