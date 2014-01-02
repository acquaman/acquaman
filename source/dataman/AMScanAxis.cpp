#include "AMScanAxis.h"

 AMScanAxis::~AMScanAxis(){}
AMScanAxis::AMScanAxis(AMScanAxis::AxisType axisType, const AMScanAxisRegion &firstRegion, QObject *parent) :
	AMDbObject(parent)
{
	setAxisType(axisType);
	regions_.append(firstRegion);
	axisValid_ = AMScanAxis::InvalidAxis;
	checkAxisValidity();
}

AMScanAxis::AxisType AMScanAxis::axisType() const{
	return axisType_;
}

AMScanAxis::AxisValid AMScanAxis::axisValid() const{
	return axisValid_;
}

const AMOrderedList<AMScanAxisRegion> AMScanAxis::regions() const{
	return regions_;
}

const AMScanAxisRegion& AMScanAxis::regionAt(int index) const{
	return regions_.at(index);
}

int AMScanAxis::regionCount() const{
	return regions_.count();
}

AMNumber AMScanAxis::axisStart() const{
	if(axisType_ == AMScanAxis::ContinuousDwellAxis || regionCount() == 0)
		return AMNumber(AMNumber::Null);

	return regionAt(0).regionStart();
}

AMNumber AMScanAxis::axisStep() const{
	if(axisType_ != AMScanAxis::StepAxis || regionCount() == 0)
		return AMNumber(AMNumber::Null);

	if(regionCount() == 1)
		return regionAt(0).regionStep();

	AMNumber firstStepSize = regionAt(0).regionStep();
	bool multipleStepSizes = firstStepSize.state() != AMNumber::Valid;
	for(int x = 1; x < regionCount(); x++)
		if(firstStepSize != regionAt(x).regionStep() || regionAt(x).regionStep().state() != AMNumber::Valid)
			multipleStepSizes = true;

	if(multipleStepSizes)
		return AMNumber(AMNumber::Null);
	return regionAt(0).regionStep();
}

AMNumber AMScanAxis::axisEnd() const{
	if(axisType_ == AMScanAxis::ContinuousDwellAxis || regionCount() == 0)
		return AMNumber(AMNumber::Null);

	return regionAt(regionCount()-1).regionEnd();
}

AMNumber AMScanAxis::axisTime() const{
	if(axisType_ == AMScanAxis::ContinuousMoveAxis || regionCount() == 0)
		return AMNumber(AMNumber::Null);

	if(regionCount() == 1)
		return regionAt(0).regionTime();

	AMNumber firstRegionTime = regionAt(0).regionTime();
	bool multipleRegionTimes = firstRegionTime.state() != AMNumber::Valid;
	for(int x = 1; x < regionCount(); x++)
		if(firstRegionTime != regionAt(x).regionTime() || regionAt(x).regionTime().state() != AMNumber::Valid)
			multipleRegionTimes = true;

	if(multipleRegionTimes)
		return AMNumber(AMNumber::Null);
	return regionAt(0).regionTime();
}

bool AMScanAxis::insertRegion(int index, const AMScanAxisRegion &region){
	if(axisType_ != AMScanAxis::StepAxis || index < 0 || index > regions_.count())
		return false;

	regions_.insert(index, region);
	if(!sanityCheckRegionsAndAxisType()){
		regions_.remove(index);
		return false;
	}
	return true;
}

bool AMScanAxis::appendRegion(const AMScanAxisRegion &region){
	if(axisType_ != AMScanAxis::StepAxis)
		return false;

	regions_.append(region);
	if(!sanityCheckRegionsAndAxisType()){
		regions_.remove(regions_.count()-1);
		return false;
	}
	return true;
}

bool AMScanAxis::overwriteRegion(int index, const AMScanAxisRegion &region){
	if(index < 0 || index > regions_.count())
		return false;

	AMScanAxisRegion oldRegion = regionAt(index);
	regions_[index] = region;
	if(sanityCheckRegionsAndAxisType()){
		regions_[index] = oldRegion;
		return false;
	}
	return true;
}

void AMScanAxis::setAxisType(AMScanAxis::AxisType axisType){
	if(axisType != AMScanAxis::StepAxis)
		while(regions_.count() > 1)
			regions_.remove(regions_.count()-1);

	axisType_ = axisType;
	checkAxisValidity();
}

bool AMScanAxis::sanityCheckRegionsAndAxisType() const{
	if(axisType_ != AMScanAxis::StepAxis && regionCount() != 1)
		return false;

	if(axisType_ == AMScanAxis::ContinuousMoveAxis){
		// We don't accepts steps
		if(regionAt(0).regionStep().state() != AMNumber::Null)
			return false;
		// We don't accept times
		if(regionAt(0).regionTime().state() != AMNumber::Null)
			return false;

		// We need a start
		if(regionAt(0).regionStart().state() == AMNumber::Null)
			return false;
		// We need an end
		if(regionAt(0).regionEnd().state() == AMNumber::Null)
			return false;
	}

	if(axisType_ == AMScanAxis::ContinuousDwellAxis){
		// We don't accept starts
		if(regionAt(0).regionStart().state() != AMNumber::Null)
			return false;
		// We don't accept steps
		if(regionAt(0).regionStep().state() != AMNumber::Null)
			return false;
		// We don't accept ends
		if(regionAt(0).regionEnd().state() != AMNumber::Null)
			return false;

		// We need a time
		if(regionAt(0).regionTime().state() == AMNumber::Null)
			return false;
	}

	if(axisType_ == AMScanAxis::ContinuousDwellAxis){
		for(int x = 0; x < regions_.count(); x++){
			// We need a start for each region
			if(regionAt(0).regionStart().state() == AMNumber::Null)
				return false;
			// We need a step for each region
			if(regionAt(0).regionStep().state() == AMNumber::Null)
				return false;
			// We need an end for each region
			if(regionAt(0).regionEnd().state() == AMNumber::Null)
				return false;
			// We need a time for each region
			if(regionAt(0).regionTime().state() == AMNumber::Null)
				return false;
		}
	}

	return true;
}

void AMScanAxis::checkAxisValidity(){
	if(sanityCheckRegionsAndAxisType())
		axisValid_ = AMScanAxis::ValidAxis;
	else
		axisValid_ = AMScanAxis::InvalidAxis;
}
