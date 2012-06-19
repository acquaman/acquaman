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


#ifndef REIXSMOVETOSAMPLETRANSFERPOSITIONACTIONINFO_H
#define REIXSMOVETOSAMPLETRANSFERPOSITIONACTIONINFO_H

#include "actions2/AMActionInfo.h"
#include "dataman/info/AMControlInfoList.h"

/// Specifies the sample manipulator position for transferring samples, used by REIXSMoveToSampleTransferPositionAction. For now, we don't save anything special in the database; positions_ is simply hard-coded into the constructor. Can add something fancier if we need to adjust it a lot.
class REIXSMoveToSampleTransferPositionActionInfo : public AMActionInfo
{
    Q_OBJECT
public:
	/// Default constructor, moves to hard-coded position defined here
	Q_INVOKABLE REIXSMoveToSampleTransferPositionActionInfo(QObject *parent = 0);
	/// Copy constructor
	REIXSMoveToSampleTransferPositionActionInfo(const REIXSMoveToSampleTransferPositionActionInfo& other);
	/// Virtual copy constructor
	AMActionInfo* createCopy() const { return new REIXSMoveToSampleTransferPositionActionInfo(*this); }


	/// Returns the sample manipulator position to move to.
	const AMControlInfoList& positions() const { return positions_; }

signals:

public slots:

protected:
	AMControlInfoList positions_;

};

#endif // REIXSMOVETOSAMPLETRANSFERPOSITIONACTIONINFO_H
