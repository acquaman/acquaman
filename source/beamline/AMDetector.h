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

protected:
	AMControl *control_;
};

class AMSpectralOutputDetector : public AMSpectralOutputDetectorInfo
{
Q_OBJECT
public:
	AMSpectralOutputDetector(const QString& name, AMControl *control, int numSpectrumBins = 0, QString xElementName = "", QStringList yElementNames = QStringList(), QObject *parent = 0);
	~AMSpectralOutputDetector() { control_ = NULL; }

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

protected:
	AMControl *reading_;
	AMControl *hvSetpoint_;
	AMControl *hvFbk_;
};

#endif // AMDETECTOR_H
