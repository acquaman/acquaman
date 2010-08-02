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

#endif // AMDETECTOR_H
