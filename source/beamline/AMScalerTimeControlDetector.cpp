#include "AMScalerTimeControlDetector.h"

AMScalerTimeControlDetector::AMScalerTimeControlDetector(const QString &name, const QString &description, AMControl *scalerTime, QObject *parent)
	: AMBasicControlDetectorEmulator(name, description, scalerTime, 0, 0, 0, AMDetectorDefinitions::ImmediateRead, parent)
{
}

AMNumber AMScalerTimeControlDetector::reading(const AMnDIndex &indexes) const
{
	if(!isConnected())
		return AMNumber(AMNumber::Null);
	// We want an "invalid" AMnDIndex for this 0D detector
	if(indexes.isValid())
		return AMNumber(AMNumber::DimensionError);

	return control_->value()/1000;
}

bool AMScalerTimeControlDetector::data(double *outputValues) const
{
	outputValues[0] = control_->value()/1000;
	return true;
}
