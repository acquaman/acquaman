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


#ifndef VESPERSPILATUSCCDDETECTORVIEW_H
#define VESPERSPILATUSCCDDETECTORVIEW_H

#include "ui/VESPERS/VESPERSCCDDetectorView.h"
#include "beamline/VESPERS/VESPERSPilatusCCDDetector.h"

#include <QProgressBar>

/// This class extends the basic detector view and adds in some file system information and enables copying all the images to the aurora drive.
class VESPERSPilatusCCDDetectorView : public VESPERSCCDDetectorView
{
	Q_OBJECT

public:
	/// Constructor.  Extends the standard CCD detector view.
	virtual ~VESPERSPilatusCCDDetectorView();
	Q_INVOKABLE explicit VESPERSPilatusCCDDetectorView(VESPERSPilatusCCDDetector *detector = 0, QWidget *parent = 0);

protected slots:
	/// Updates the progress bar and status label when new disk usage data is read.
	void onFileSystemInfoUpdate();

protected:
	/// A progresss bar that shows the disk usage.
	QProgressBar *diskUsageBar_;
	/// Label that has a light in it to indicate relative fullness.  Green from 0-60%, yellow from 60-80%, and red for >80%.
	QLabel *status_;
	/// Label that holds the ratio of disk space occupied vs total.
	QLabel *spaceLabel_;
};

#endif // VESPERSPILATUSCCDDETECTORVIEW_H
