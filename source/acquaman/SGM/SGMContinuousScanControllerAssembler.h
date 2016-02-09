#ifndef SGMCONTINUOUSSCANCONTROLLERASSEMBLER_H
#define SGMCONTINUOUSSCANCONTROLLERASSEMBLER_H

#include "acquaman/AMGenericScanActionControllerAssembler.h"

class SGMContinuousScanControllerAssembler : public AMGenericScanActionControllerAssembler
{
    Q_OBJECT
public:
	explicit SGMContinuousScanControllerAssembler(bool automaticDirectionAssessment, AMScanConfiguration::Direction direction, QObject *parent = 0);

	virtual ~SGMContinuousScanControllerAssembler() {}


signals:

public slots:
protected:
	/// Adds all the non-scaler detectors, plus one instance of a scaler detector.
	virtual QList<AMDetector*> generateListOfDetectorsToConfigure() const;

};

#endif // SGMCONTINUOUSSCANCONTROLLERASSEMBLER_H
