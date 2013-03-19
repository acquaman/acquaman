#include "AMScanActionControllerScanAssembler.h"

AMScanAxisRegion::AMScanAxisRegion(AMNumber start, AMNumber step, AMNumber end, AMNumber time, QObject *parent) :
	AMDbObject(parent)
{
	setRegionStart(start);
	setRegionStep(step);
	setRegionEnd(end);
	setRegionTime(time);
}

AMNumber AMScanAxisRegion::regionStart() const{
	return regionStart_;
}

AMNumber AMScanAxisRegion::regionStep() const{
	return regionStep_;
}

AMNumber AMScanAxisRegion::regionEnd() const{
	return regionEnd_;
}

AMNumber AMScanAxisRegion::regionTime() const{
	return regionTime_;
}

void AMScanAxisRegion::setRegionStart(AMNumber regionStart){
	regionStart_ = regionStart;
}

void AMScanAxisRegion::setRegionStep(AMNumber regionStep){
	regionStep_ = regionStep;
}

void AMScanAxisRegion::setRegionEnd(AMNumber regionEnd){
	regionEnd_ = regionEnd;
}

void AMScanAxisRegion::setRegionTime(AMNumber regionTime){
	regionTime_ = regionTime;
}

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

AMScanActionControllerScanAssembler::AMScanActionControllerScanAssembler(QObject *parent) :
	QObject(parent)
{
	detectors_ = new AMDetectorSet(this);
	controls_ = new AMControlSet(this);
}

bool AMScanActionControllerScanAssembler::insertAxis(int index, AMControl *axisControl, AMScanAxis *scanAxis){
	if(!axisControl || !scanAxis)
		return false;
	if(controls_->contains(axisControl->name()) || axes_.contains(scanAxis))
		return false;
	if(index < 0 || index > controls_->count())
		return false;
	if(controls_->count() != axes_.count())
		return false;

	controls_->insert(index, axisControl, axisControl->name());
	axes_.insert(index, scanAxis);
	return true;
}

bool AMScanActionControllerScanAssembler::appendAxis(AMControl *axisControl, AMScanAxis *scanAxis){
	return insertAxis(controls_->count(), axisControl, scanAxis);
}

bool AMScanActionControllerScanAssembler::addDetector(AMDetector *detector){
	return detectors_->addDetector(detector);
}

#include "actions3/AMListAction3.h"
AMAction3* AMScanActionControllerScanAssembler::generateActionTree(){
	qDebug() << "Axes in reverse order";
	for(int x = axes_.count()-1; x >= 0; x--){
		qDebug() << controls_->at(x)->name() << "Start: " << axes_.at(x)->axisStart().toString() << "Delta: " << axes_.at(x)->axisStep().toString() << "End: " << axes_.at(x)->axisEnd().toString();
		if(axes_.at(x)->regionCount() > 1)
			for(int y = 0; y < axes_.at(x)->regionCount(); y++)
				qDebug() << "\tStart: " << axes_.at(x)->regionAt(y).regionStart().toString() << "Delta: " << axes_.at(x)->regionAt(y).regionStep().toString() << "End: " << axes_.at(x)->regionAt(y).regionEnd().toString();
	}

	AMAction3 *retVal = generateActionTreeForAxis(controls_->at(0), axes_.at(0));

	QList<AMAction3*> insertionPoints = findInsertionPoints(retVal);
	for(int x = 1; x < axes_.count(); x++){
		QList<AMAction3*> newInsertionPoints;
		newInsertionPoints.clear();

		for(int y = 0; y < insertionPoints.count(); y++){
			AMListAction3 *castParentToListAction = qobject_cast<AMListAction3*>(insertionPoints.at(y)->parentAction());
			if(castParentToListAction){
				int indexOfAction = castParentToListAction->indexOfSubAction(insertionPoints.at(y));
				castParentToListAction->insertSubAction(generateActionTreeForAxis(controls_->at(x), axes_.at(x)), indexOfAction);
				castParentToListAction->deleteSubAction(indexOfAction+1);
				newInsertionPoints.append(findInsertionPoints(castParentToListAction->subActionAt(indexOfAction)));
			}
		}
		insertionPoints.clear();
		insertionPoints = newInsertionPoints;
	}

	QList<AMAction3*> detectorInsertionPoints = findInsertionPoints(retVal);
	qDebug() << "Found " << detectorInsertionPoints.count() << " detector insertion points";
	for(int x = 0; x < detectorInsertionPoints.count(); x++){
		AMListAction3 *castParentToListAction = qobject_cast<AMListAction3*>(detectorInsertionPoints.at(x)->parentAction());
		if(castParentToListAction){
			int indexOfAction = castParentToListAction->indexOfSubAction(detectorInsertionPoints.at(x));
			castParentToListAction->insertSubAction(generateActionListForDetectorAcquisition(), indexOfAction);
			castParentToListAction->deleteSubAction(indexOfAction+1);
		}
	}

	AMListAction3 *castRetValToListAction = qobject_cast<AMListAction3*>(retVal);
	if(castRetValToListAction){
		AMListAction3 *castFirstToListAction = qobject_cast<AMListAction3*>(castRetValToListAction->subActionAt(0));
		if(castFirstToListAction)
			castFirstToListAction->addSubAction(generateActionListForDetectorInitialization());

		AMListAction3 *castLastToListAction = qobject_cast<AMListAction3*>(castRetValToListAction->subActionAt(castRetValToListAction->subActionCount()-1));
		if(castLastToListAction)
			castLastToListAction->addSubAction(generateActionListForDetectorCleanup());
	}

	return retVal;
}

AMAction3* AMScanActionControllerScanAssembler::generateActionTreeForAxis(AMControl *axisControl, AMScanAxis *scanAxis){
	if(scanAxis->axisType() == AMScanAxis::StepAxis)
		return generateActionTreeForStepAxis(axisControl, scanAxis);
	else if(scanAxis->axisType() == AMScanAxis::ContinuousMoveAxis)
		return generateActionTreeForContinuousMoveAxis(axisControl, scanAxis);
	else if(scanAxis->axisType() == AMScanAxis::ContinuousDwellAxis)
		return generateActionTreeForContinuousDwellAxis(axisControl, scanAxis);
	else
		return 0; //NULL
}

#include "actions3/AMLoopAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"
AMAction3* AMScanActionControllerScanAssembler::generateActionTreeForStepAxis(AMControl *axisControl, AMScanAxis *stepScanAxis){
	AMListAction3 *axisActions = new AMListAction3(new AMListActionInfo3(QString("Axis %1").arg(axisControl->name()), QString("Axis %1").arg(axisControl->name())), AMListAction3::Sequential);

	// generate axis initialization list
	AMListAction3 *initializationActions = new AMListAction3(new AMListActionInfo3(QString("Initializing %1").arg(axisControl->name()), QString("Initializing Axis with Control %1").arg(axisControl->name())), AMListAction3::Sequential);
	AMControlInfo initializeControlPositionSetpoint = axisControl->toInfo();
	initializeControlPositionSetpoint.setValue(stepScanAxis->axisStart());
	AMControlMoveAction3 *initializeControlPosition = new AMControlMoveAction3(new AMControlMoveActionInfo3(initializeControlPositionSetpoint), axisControl);
	initializationActions->addSubAction(initializeControlPosition);

	AMListAction3 *allRegionsList = new AMListAction3(new AMListActionInfo3(QString("%1 Regions for %2 Axis").arg(stepScanAxis->regionCount()).arg(axisControl->name()), QString("%1 Regions for %2 Axis").arg(stepScanAxis->regionCount()).arg(axisControl->name())), AMListAction3::Sequential);
	for(int x = 0; x < stepScanAxis->regionCount(); x++)
		allRegionsList->addSubAction(generateActionTreeForStepAxisRegion(axisControl, stepScanAxis->regionAt(x)));

	// generate axis cleanup list
	AMListAction3 *cleanupActions = new AMListAction3(new AMListActionInfo3(QString("Cleaning Up %1").arg(axisControl->name()), QString("Cleaning Up Axis with Control %1").arg(axisControl->name())), AMListAction3::Sequential);

	axisActions->addSubAction(initializationActions);
	axisActions->addSubAction(allRegionsList);
	axisActions->addSubAction(cleanupActions);
	return axisActions;
}

AMAction3* AMScanActionControllerScanAssembler::generateActionTreeForStepAxisRegion(AMControl *axisControl, const AMScanAxisRegion &stepScanAxisRegion){
	AMListAction3 *regionList = new AMListAction3(new AMListActionInfo3(QString("Region on %1").arg(axisControl->name()), QString("Region from %1 to %2 by %3 on %4").arg(stepScanAxisRegion.regionStart().toString()).arg(stepScanAxisRegion.regionEnd().toString()).arg(stepScanAxisRegion.regionStep().toString()).arg(axisControl->name())), AMListAction3::Sequential);
	AMControlInfo regionStartSetpoint = axisControl->toInfo();
	regionStartSetpoint.setValue(stepScanAxisRegion.regionStart());
	AMControlMoveAction3 *regionStart = new AMControlMoveAction3(new AMControlMoveActionInfo3(regionStartSetpoint), axisControl);

	// generate axis loop for region
	int loopIterations = ceil(( ((double)stepScanAxisRegion.regionEnd()) - ((double)stepScanAxisRegion.regionStart()) )/ ((double)stepScanAxisRegion.regionStep()) );
	AMLoopAction3 *axisLoop = new AMLoopAction3(new AMLoopActionInfo3(loopIterations, QString("Loop %1").arg(axisControl->name()), QString("Looping from %1 to %2 by %3 on %4").arg(stepScanAxisRegion.regionStart().toString()).arg(stepScanAxisRegion.regionEnd().toString()).arg(stepScanAxisRegion.regionStep().toString()).arg(axisControl->name())));
	AMListAction3 *nextLevelHolderAction = new AMListAction3(new AMListActionInfo3("Holder Action for the Next Sublevel", "Holder Action for the Next Sublevel"));
	AMControlInfo controlLoopMoveInfoSetpoint = axisControl->toInfo();
	controlLoopMoveInfoSetpoint.setValue(stepScanAxisRegion.regionStep());
	AMControlMoveActionInfo3 *controlLoopMoveInfo = new AMControlMoveActionInfo3(controlLoopMoveInfoSetpoint);
	controlLoopMoveInfo->setIsRelativeMove(true);
	AMControlMoveAction3 *controlLoopMove = new AMControlMoveAction3(controlLoopMoveInfo, axisControl);
	axisLoop->addSubAction(nextLevelHolderAction);
	axisLoop->addSubAction(controlLoopMove);

	regionList->addSubAction(regionStart);
	regionList->addSubAction(axisLoop);

	return regionList;
}

AMAction3* AMScanActionControllerScanAssembler::generateActionTreeForContinuousMoveAxis(AMControl *axisControl, AMScanAxis *continuiousMoveScanAxis){
	return 0; //NULL
}

AMAction3* AMScanActionControllerScanAssembler::generateActionTreeForContinuousDwellAxis(AMControl *axisControl, AMScanAxis *continuousDwellScanAxis){
	return 0; //NULL
}

AMAction3* AMScanActionControllerScanAssembler::generateActionListForDetectorAcquisition(){
	if(axes_.at(axes_.count()-1)->axisType() == AMScanAxis::StepAxis)
		return generateActionListForStepDetectorAcquisition();
	else if( (axes_.at(axes_.count()-1)->axisType() == AMScanAxis::ContinuousMoveAxis) || (axes_.at(axes_.count()-1)->axisType() == AMScanAxis::ContinuousDwellAxis) )
		return generateActionListForContinuousDetectorAcquisition();
	else
		return 0; //NULL
}

AMAction3* AMScanActionControllerScanAssembler::generateActionListForStepDetectorAcquisition(){
	AMListAction3 *retVal = new AMListAction3(new AMListActionInfo3(QString("Acquire Detectors"), QString("Acquire %1 Detectors").arg(detectors_->count())), AMListAction3::Parallel);

	for(int x = 0; x < detectors_->count(); x++)
		retVal->addSubAction(detectors_->at(x)->createAcquisitionAction(AMDetectorDefinitions::SingleRead));

	return retVal;
}

AMAction3* AMScanActionControllerScanAssembler::generateActionListForContinuousDetectorAcquisition(){
	AMListAction3 *retVal = new AMListAction3(new AMListActionInfo3(QString("Acquire Detectors"), QString("Acquire %1 Detectors").arg(detectors_->count())), AMListAction3::Parallel);

	for(int x = 0; x < detectors_->count(); x++)
		retVal->addSubAction(detectors_->at(x)->createAcquisitionAction(AMDetectorDefinitions::ContinuousRead));

	return retVal;
}

AMAction3* AMScanActionControllerScanAssembler::generateActionListForDetectorInitialization(){
	AMListAction3 *retVal = new AMListAction3(new AMListActionInfo3(QString("Initialize Detectors"), QString("Initialize %1 Detectors").arg(detectors_->count())), AMListAction3::Parallel);

	for(int x = 0; x < detectors_->count(); x++)
		retVal->addSubAction(detectors_->at(x)->createInitializationActions());

	return retVal;
}

AMAction3* AMScanActionControllerScanAssembler::generateActionListForDetectorCleanup(){
	AMListAction3 *retVal = new AMListAction3(new AMListActionInfo3(QString("Cleanup Detectors"), QString("Cleanup %1 Detectors").arg(detectors_->count())), AMListAction3::Parallel);

	for(int x = 0; x < detectors_->count(); x++)
		retVal->addSubAction(detectors_->at(x)->createCleanupActions());

	return retVal;
}

QList<AMAction3*> AMScanActionControllerScanAssembler::findInsertionPoints(AMAction3 *action){
	QList<AMAction3*> retVal;
	AMListAction3 *castToListAction = qobject_cast<AMListAction3*>(action);
	if(castToListAction){
		for(int x = 0; x < castToListAction->subActionCount(); x++){
			AMListAction3 *subActionToListAction = qobject_cast<AMListAction3*>(castToListAction->subActionAt(x));
			if(subActionToListAction && subActionToListAction->info()->shortDescription() == "Holder Action for the Next Sublevel")
				retVal.append(subActionToListAction);
			retVal.append((findInsertionPoints(castToListAction->subActionAt(x))));
		}
	}
	return retVal;
}
