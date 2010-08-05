#include "AMDetector.h"


AMSingleControlDetector::AMSingleControlDetector(const QString& name, AMControl *control, QObject *parent) :
		AMDetectorInfo(name, name, parent)
{
	control_ = control;
}

AMSpectralOutputDetector::AMSpectralOutputDetector(const QString& name, AMControl *control, int numSpectrumBins, QString xElementName, QStringList yElementNames, QObject *parent) :
		AMSpectralOutputDetectorInfo(name, name, numSpectrumBins, xElementName, yElementNames, parent)
{
	control_ = control;
}

PGTDetector::PGTDetector(const QString &name, AMControl *dataWaveform, AMControl *hvSetpoint, AMControl *hvFbk, AMControl *integrationTime, AMControl *integrationMode, QObject *parent) :
		PGTDetectorInfo(name, name, parent)
{
	dataWaveform_ = dataWaveform;
	hvSetpoint_ = hvSetpoint;
	hvFbk_ = hvFbk;
	integrationTime_ = integrationTime;
	integrationMode_ = integrationMode;
}

PGTDetector::~PGTDetector()
{
	dataWaveform_ = NULL;
	hvSetpoint_ = NULL;
	hvFbk_ = NULL;
	integrationTime_ = NULL;
	integrationMode_ = NULL;
}

MCPDetector::MCPDetector(const QString &name, AMControl *reading, AMControl *hvSetpoint, AMControl *hvFbk, QObject *parent) :
		MCPDetectorInfo(name, name, parent)
{
	reading_ = reading;
	hvSetpoint_ = hvSetpoint;
	hvFbk_ = hvFbk;
}

MCPDetector::~MCPDetector(){
	reading_ = NULL;
	hvSetpoint_ = NULL;
	hvFbk_ = NULL;
}
