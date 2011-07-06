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


#ifndef AMSCANCONFIGURATIONVIEWER_H
#define AMSCANCONFIGURATIONVIEWER_H

#include <QtGui>

#include "acquaman/AMScanConfiguration.h"
#include "acquaman/AMScanController.h"

class AMScanConfigurationViewer : public QWidget
{
	Q_OBJECT
public:
    AMScanConfigurationViewer(AMScanConfiguration *cfg, QWidget *parent = 0);

signals:
	void scanControllerReady(AMScanController *xasCtrl);
	void startScanRequested();
	void addToQueueRequested();
	void queueDirectorRequested();

protected slots:
	virtual void onStartScanClicked(){ emit startScanRequested(); }
	virtual void onAddToQueueRequested() { emit addToQueueRequested(); }
	virtual void onLockdowScanning(bool isLocked, QString reason) = 0;

protected:
    AMScanConfiguration *cfg_;
};

#endif // AMSCANCONFIGURATIONVIEWER_H
