#ifndef BIOXASGENERICSTEPSCANCONTROLLER_H
#define BIOXASGENERICSTEPSCANCONTROLLER_H

#include "acquaman/AMGenericStepScanController.h"
#include "acquaman/BioXAS/BioXASGenericStepScanConfiguration.h"

class BioXASGenericStepScanController : public AMGenericStepScanController
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASGenericStepScanController(BioXASGenericStepScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASGenericStepScanController();

protected:
	/// Adds anything extra (eg: analysis blocks) to the scan before it's started.
	virtual void buildScanControllerImplementation();

	/// Creates a region of interest analysis block from the given AMRegionOfInterest and 1D spectrum source.
	AMAnalysisBlock *createRegionOfInterestAB(const QString &name, AMRegionOfInterest *region, AMDataSource *spectrumSource) const;
	/// Creates a normalized analysis block from a source and normalizer source.
	AMAnalysisBlock *createNormalizationAB(const QString &name, AMDataSource *source, AMDataSource *normalizer) const;
};

#endif // BIOXASGENERICSTEPSCANCONTROLLER_H
