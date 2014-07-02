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


#include "VESPERSPilatusCCDDetector.h"

VESPERSPilatusCCDDetector::~VESPERSPilatusCCDDetector(){}

VESPERSPilatusCCDDetector::VESPERSPilatusCCDDetector(const QString &name, const QString &description, QObject *parent)
	: VESPERSCCDDetector(name, description, parent)
{
	axes_ << AMAxisInfo("X-axis", 981, "X dimension in pixels", "Counts")
		  << AMAxisInfo("Y-axis", 1043, "Y dimension in pixels", "Counts");

	acquireControl_ = new AMPVControl("Acquisition", "PAD1607-B21-05:cam1:Acquire", "PAD1607-B21-05:cam1:Acquire", "PAD1607-B21-05:cam1:Acquire", this, 0.1, 10.0, 0);
	acquireTimeControl_ = new AMSinglePVControl("Acquire Time", "PAD1607-B21-05:cam1:AcquireTime", this, 0.1);
	exposurePeriodControl_ = new AMSinglePVControl("Exposure Period", "PAD1607-B21-05:cam1:AcquirePeriod", this, 0.1);
	acquisitionStatusControl_ = new AMSinglePVControl("Detector Status", "PAD1607-B21-05:cam1:Acquire", this, 0.1);
	ccdFilePathControl_ = new AMSinglePVControl("File Path", "PAD1607-B21-05:cam1:FilePath", this);
	ccdFileBaseNameControl_ = new AMSinglePVControl("File Base Name", "PAD1607-B21-05:cam1:FileName", this);
	ccdFileNumberControl_ = new AMSinglePVControl("File Number","PAD1607-B21-05:cam1:FileNumber", this);

	allControlsCreated();

	allControls_->addControl(exposurePeriodControl_);

	dfProcess_ = new QProcess;
	connect(dfProcess_, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(readOutProcess(int,QProcess::ExitStatus)));
	connect(this, SIGNAL(connected(bool)), this, SLOT(updateAuroraSize()));

	QTimer updateAuroraSizeCounter;
	connect(&updateAuroraSizeCounter, SIGNAL(timeout()), this, SLOT(updateAuroraSize()));
	updateAuroraSizeCounter.start(300000);
}

bool VESPERSPilatusCCDDetector::setAcquisitionTime(double seconds)
{
	if(!isConnected())
		return false;

	if(!acquireTimeControl_->withinTolerance(seconds)){

		acquireTimeControl_->move(seconds);
		exposurePeriodControl_->move(seconds+0.0023);
	}

	return true;
}

void VESPERSPilatusCCDDetector::updateAuroraSize()
{
	dfProcess_->start("df", QStringList() << "/nas/vespers");
}

void VESPERSPilatusCCDDetector::readOutProcess(int exitCode, QProcess::ExitStatus exitStatus)
{
	if (exitCode == 0 && exitStatus == QProcess::NormalExit){

		QStringList output = QString(dfProcess_->readAll()).split(" ", QString::SkipEmptyParts);
		totalAuroraSize_ = output.at(7).toDouble()/1e9;
		occupiedAuroraSize_ = output.at(8).toDouble()/1e9;
		emit fileSystemInfoUpdated();
	}
}

QString VESPERSPilatusCCDDetector::synchronizedDwellKey() const
{
	return "PAD1607-B21-05:cam1:Acquire NPP NMS";
}
