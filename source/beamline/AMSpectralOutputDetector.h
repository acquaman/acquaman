#ifndef AMSPECTRALOUTPUTDETECTOR_H
#define AMSPECTRALOUTPUTDETECTOR_H

#include "AMDetector.h"
#include "dataman/AMSpectralOutputDetectorInfo.h"

class AMSpectralOutputDetector : public AMSpectralOutputDetectorInfo, public AMDetector
{
Q_OBJECT
public:
	AMSpectralOutputDetector(const QString& name, AMControl *control, int numSpectrumBins = 0, QString xElementName = "", QStringList yElementNames = QStringList(), QObject *parent = 0);
	~AMSpectralOutputDetector();

	const QMetaObject* getMetaObject();
	AMDetectorInfo toInfo();

	bool setFromInfo(const AMDetectorInfo &info);

public slots:
	virtual bool setControls(AMSpectralOutputDetectorInfo *detectorSettings);

protected:
	AMControl *control_;
};

#endif // AMSPECTRALOUTPUTDETECTOR_H
