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


#ifndef ACQMAN_AMDETECTOR_H
#define ACQMAN_AMDETECTOR_H

#include <QObject>
#include "dataman/AMDetectorInfo.h"
#include "AMControl.h"

/// \todo Comment this file, add detector description, and flesh out into actual hardware detectors.
class AMSingleControlDetector : public AMDetectorInfo
{
Q_OBJECT
public:
	AMSingleControlDetector(const QString& name, AMControl *control, QObject *parent = 0);
	~AMSingleControlDetector(){ control_ = NULL;}

public slots:
	virtual bool setControls(AMDetectorInfo *detectorSettings);

protected:
	AMControl *control_;
};

class AMSpectralOutputDetector : public AMSpectralOutputDetectorInfo
{
Q_OBJECT
public:
	AMSpectralOutputDetector(const QString& name, AMControl *control, int numSpectrumBins = 0, QString xElementName = "", QStringList yElementNames = QStringList(), QObject *parent = 0);
	~AMSpectralOutputDetector() { control_ = NULL; }

public slots:
	virtual bool setControls(AMSpectralOutputDetectorInfo *detectorSettings);

protected:
	AMControl *control_;
};

class PGTDetector : public PGTDetectorInfo
{
	Q_OBJECT
public:
	PGTDetector(const QString& name, AMControl *dataWaveform, AMControl *hvSetpoint, AMControl *hvFbk, AMControl *integrationTime, AMControl *integrationMode, QObject *parent = 0);
	~PGTDetector();

	AMControl* dataWaveformCtrl() const { return dataWaveform_; }
	AMControl* hvSetpointCtrl() const { return hvSetpoint_; }
	AMControl* hvFbkCtrl() const { return hvFbk_; }
	AMControl* integrationTimeCtrl() const { return integrationTime_; }
	AMControl* integrationModeCtrl() const { return integrationMode_; }

	bool settingsMatchFbk(PGTDetectorInfo* settings);

public slots:
	virtual bool setControls(PGTDetectorInfo *pgtSettings);

protected:
	AMControl *dataWaveform_;
	AMControl *hvSetpoint_;
	AMControl *hvFbk_;
	AMControl *integrationTime_;
	AMControl *integrationMode_;
};

class MCPDetector : public MCPDetectorInfo
{
	Q_OBJECT
public:
	MCPDetector(const QString& name, AMControl *reading, AMControl *hvSetpoint, AMControl *hvFbk, QObject *parent = 0);
	~MCPDetector();

	AMControl* hvSetpointCtrl() const { return hvSetpoint_; }
	AMControl* hvFbkCtrl() const { return hvFbk_; }

	bool settingsMatchFbk(MCPDetectorInfo* settings);

public slots:
	virtual bool setControls(MCPDetectorInfo *mcpSettings);

protected:
	AMControl *reading_;
	AMControl *hvSetpoint_;
	AMControl *hvFbk_;
};

#endif // AMDETECTOR_H
