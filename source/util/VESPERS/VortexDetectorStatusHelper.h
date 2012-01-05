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


#ifndef VORTEXDETECTORSTATUSHELPER_H
#define VORTEXDETECTORSTATUSHELPER_H

#include <QObject>

#include "beamline/AMProcessVariable.h"

class VortexDetectorStatusHelper : public QObject
{
	Q_OBJECT
public:
	/// Constructor that sets up some PVs and connects them to others.  This is required because of naming convention problems between similar detectors used on VESPERS.
	explicit VortexDetectorStatusHelper(QObject *parent = 0);

signals:

public slots:

protected slots:
	/// Determines if the new single element MCA status value needs to be propogated.
	void setSingleMCA(int val) { if (singleMCAto_->getInt() != singleMCAfrom_->getInt()) singleMCAfrom_->setValue(val); }
	/// Determines if the new single element DXP status value needs to be propogated.
	void setSingleDXP(int val) { if (singleDXPto_->getInt() != singleDXPfrom_->getInt()) singleDXPfrom_->setValue(val); }
	/// Determines if the new four element MCA status value needs to be propogated.
	void setFourMCA(int val) { if (fourMCAto_->getInt() != fourMCAfrom_->getInt()) fourMCAfrom_->setValue(val); }
	/// Determines if the new four element DXP status value needs to be propogated.
	void setFourDXP(int val) { if (fourDXPto_->getInt() != fourDXPfrom_->getInt()) fourDXPfrom_->setValue(val); }
	/// Determines if the new four element Emax value needs to be propogated.
	void setFourEMax(double val) { if (fourEMaxTo_->getDouble() != fourEMaxFrom_->getDouble()) fourEMaxTo_->setValue(val); }
	/// Determines if the new four element peaking time value needs to be propogated.
	void setFourPeakTime(double val) { if (fourPeakTimeTo_->getDouble() != fourPeakTimeFrom_->getDouble()) fourPeakTimeTo_->setValue(val); }

protected:
	/// PVs for the different things that need connection.
	AMProcessVariable *singleMCAto_;
	AMProcessVariable *singleMCAfrom_;

	AMProcessVariable *singleDXPto_;
	AMProcessVariable *singleDXPfrom_;

	AMProcessVariable *fourMCAto_;
	AMProcessVariable *fourMCAfrom_;

	AMProcessVariable *fourDXPto_;
	AMProcessVariable *fourDXPfrom_;

	AMProcessVariable *fourEMaxTo_;
	AMProcessVariable *fourEMaxFrom_;

	AMProcessVariable *fourPeakTimeTo_;
	AMProcessVariable *fourPeakTimeFrom_;
};

#endif // VORTEXDETECTORSTATUSHELPER_H
