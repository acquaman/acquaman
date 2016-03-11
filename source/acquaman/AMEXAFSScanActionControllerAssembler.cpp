/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#include "AMEXAFSScanActionControllerAssembler.h"

#include "actions3/AMListAction3.h"
#include "actions3/AMLoopAction3.h"
#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMAxisStartedAction.h"
#include "actions3/actions/AMAxisFinishedAction.h"
#include "actions3/actions/AMAxisValueFinishedAction.h"
#include "beamline/AMDetectorTriggerSource.h"
#include "util/AMErrorMonitor.h"
#include "util/AMVariableIntegrationTime.h"
#include "util/AMEnergyToKSpaceCalculator.h"

AMEXAFSScanActionControllerAssembler::AMEXAFSScanActionControllerAssembler(QObject *parent)
	: AMGenericScanActionControllerAssembler(false, AMScanConfiguration::Increase, parent)
{
}

bool AMEXAFSScanActionControllerAssembler::generateActionTreeImplmentation()
{
	// There is only one axis, but there can be many regions in an EXAFS scan.
	QList<AMScanAxisEXAFSRegion *> exafsRegions;

	foreach (AMScanAxisRegion *region, axes_.at(0)->regions().toList()){

		AMScanAxisEXAFSRegion *exafsRegion = qobject_cast<AMScanAxisEXAFSRegion *>(region);

		if (exafsRegion)
			exafsRegions << exafsRegion;

		else {

			AMErrorMon::alert(this, AMEXAFSSCANACTIONCONTROLLERASSEMBLER_AXES_INVALID, "An axis region was invalid.");
			return false;
		}
	}

	// First, if there are no special extended regions of any kind, just use the generic assembler.
	bool hasExtendedRegion = false;

	foreach (AMScanAxisEXAFSRegion *region, exafsRegions)
		if (region->inKSpace())
			hasExtendedRegion = true;

	// If no EXAFS specific regions are required.  Just make it a normal step scan.
	if (!hasExtendedRegion)
		return AMGenericScanActionControllerAssembler::generateActionTreeImplmentation();

	// Build an EXAFS scan.
	AMScanAxis *axis = axes_.at(0);
	AMControl *axisControl = controls_->at(0);
	AMListAction3 *axisActions = new AMListAction3(new AMListActionInfo3(QString("Axis %1").arg(axis->name()), QString("Axis %1").arg(axis->name())), AMListAction3::Sequential);

	AMAxisStartedAction *axisStartAction = new AMAxisStartedAction(new AMAxisStartedActionInfo(QString("%1 Axis").arg(axis->name()), AMScanAxis::StepAxis));
	axisActions->addSubAction(axisStartAction);

	if (axisControl){

		// generate axis initialization list
		AMListAction3 *initializationActions = new AMListAction3(new AMListActionInfo3(QString("Initializing %1").arg(axisControl->name()), QString("Initializing Axis with Control %1").arg(axisControl->name())), AMListAction3::Sequential);

		AMAction3 *initializeControlPosition = AMActionSupport::buildControlMoveAction(axisControl, axis->axisStart());
		initializeControlPosition->setGenerateScanActionMessage(true);
		initializationActions->addSubAction(initializeControlPosition);
		axisActions->addSubAction(initializationActions);
	}

	AMListAction3 *allRegionsList = new AMListAction3(new AMListActionInfo3(QString("%1 Regions for %2 Axis").arg(axis->regionCount()).arg(axis->name()), QString("%1 Regions for %2 Axis").arg(axis->regionCount()).arg(axis->name())), AMListAction3::Sequential);

	for(int i = 0, size = exafsRegions.size(); i < size; i++){

		AMScanAxisEXAFSRegion *exafsRegion = exafsRegions.at(i);

		if (exafsRegion->inKSpace())
			allRegionsList->addSubAction(generateActionTreeForEXAFSStepAxisRegion(axisControl, exafsRegion, (i == size-1)));

		else
			allRegionsList->addSubAction(generateActionTreeForStepAxisRegion(axisControl, exafsRegion, (i == size-1) ));
	}

	axisActions->addSubAction(allRegionsList);

	if (axisControl){

		// generate axis cleanup list
		AMListAction3 *cleanupActions = new AMListAction3(new AMListActionInfo3(QString("Cleaning Up %1").arg(axisControl->name()), QString("Cleaning Up Axis with Control %1").arg(axisControl->name())), AMListAction3::Sequential);
		axisActions->addSubAction(cleanupActions);
	}

	AMAxisFinishedAction *axisFinishAction = new AMAxisFinishedAction(new AMAxisFinishedActionInfo(QString("%1 Axis").arg(axis->name())));
	axisActions->addSubAction(axisFinishAction);

	actionTree_ = axisActions;

	QList<AMAction3*> detectorInsertionPoints = findInsertionPoints(actionTree_);

	for(int x = 0; x < detectorInsertionPoints.count(); x++){

		AMListAction3 *castParentToListAction = qobject_cast<AMListAction3*>(detectorInsertionPoints.at(x)->parentAction());

		if(castParentToListAction){

			int indexOfAction = castParentToListAction->indexOfSubAction(detectorInsertionPoints.at(x));
			castParentToListAction->insertSubAction(generateActionListForDetectorAcquisition(), indexOfAction);
			castParentToListAction->deleteSubAction(indexOfAction+1);
		}
	}

	AMListAction3 *castRetValToListAction = qobject_cast<AMListAction3*>(actionTree_);

	if(castRetValToListAction){

		AMListAction3 *castFirstToListAction = qobject_cast<AMListAction3*>(castRetValToListAction->subActionAt(1));

		if(castFirstToListAction)
			castFirstToListAction->addSubAction(generateActionListForDetectorInitialization());

		AMListAction3 *castLastToListAction = qobject_cast<AMListAction3*>(castRetValToListAction->subActionAt(castRetValToListAction->subActionCount()-2));

		if(castLastToListAction)
			castLastToListAction->addSubAction(generateActionListForDetectorCleanup());
	}

	return true;
}

AMAction3 *AMEXAFSScanActionControllerAssembler::generateActionTreeForEXAFSStepAxisRegion(AMControl *axisControl, AMScanAxisEXAFSRegion *exafsRegion, bool isFinalRegion)
{
	AMListAction3 *regionList = new AMListAction3(new AMListActionInfo3(QString("Region on %1").arg(exafsRegion->name()), QString("Region from %1 to %2 by %3 on %4").arg(exafsRegion->regionStart().toString()).arg(exafsRegion->regionEnd().toString()).arg(exafsRegion->regionStep().toString()).arg(exafsRegion->name())), AMListAction3::Sequential);

	// If there is no difference in time and maximumTime or if maximumTime is invalid then we only need to set the time once.
	if (!exafsRegion->maximumTime().isValid() || double(exafsRegion->regionTime()) == double(exafsRegion->maximumTime())){

		AMListAction3 *detectorSetDwellList = new AMListAction3(new AMListActionInfo3(QString("Set All Detectors Dwell Times"), QString("Set %1 Detectors").arg(detectors_->count())), AMListAction3::Parallel);

		for(int x = 0; x < detectors_->count(); x++){

			AMAction3 *detectorSetDwellAction = detectors_->at(x)->createSetAcquisitionTimeAction(exafsRegion->regionTime());

			if(detectorSetDwellAction)
				detectorSetDwellList->addSubAction(detectorSetDwellAction);

		}

		regionList->addSubAction(detectorSetDwellList);

		int extendedPoints = int(round(( ((double)exafsRegion->regionEnd()) - ((double)exafsRegion->regionStart()) )/ ((double)exafsRegion->regionStep()) ));
		QVector<double> energyPositions = QVector<double>(extendedPoints);
		AMEnergyToKSpaceCalculator calculator = AMEnergyToKSpaceCalculator(exafsRegion->edgeEnergy());
		calculator.energyValues(exafsRegion->regionStart(), exafsRegion->regionStep(), exafsRegion->regionEnd(), energyPositions.data());

		for (int i = 0; i < extendedPoints; i++){

			AMAction3 *controlMove = AMActionSupport::buildControlMoveAction(axisControl, energyPositions.at(i), false);
			controlMove->setGenerateScanActionMessage(true);
			regionList->addSubAction(controlMove);
			AMListAction3 *nextLevelHolderAction = new AMListAction3(new AMListActionInfo3("Holder Action for the Next Sublevel", "Holder Action for the Next Sublevel"));
			regionList->addSubAction(nextLevelHolderAction);
			AMAxisValueFinishedActionInfo *axisValueFinishedInfo = new AMAxisValueFinishedActionInfo;
			axisValueFinishedInfo->setShortDescription(QString("%1 axis value finshed").arg(exafsRegion->name()));
			AMAxisValueFinishedAction *axisValueFinishedAction = new AMAxisValueFinishedAction(axisValueFinishedInfo);
			regionList->addSubAction(axisValueFinishedAction);
		}

		if (isFinalRegion){

			AMAction3 *controlMove = AMActionSupport::buildControlMoveAction(axisControl, double(calculator.energy(exafsRegion->regionEnd())), false);
			controlMove->setGenerateScanActionMessage(true);
			regionList->addSubAction(controlMove);
			AMListAction3 *nextLevelHolderAction = new AMListAction3(new AMListActionInfo3("Holder Action for the Next Sublevel", "Holder Action for the Next Sublevel"));
			regionList->addSubAction(nextLevelHolderAction);
			AMAxisValueFinishedActionInfo *axisValueFinishedInfo = new AMAxisValueFinishedActionInfo;
			axisValueFinishedInfo->setShortDescription(QString("%1 axis value finshed").arg(exafsRegion->name()));
			AMAxisValueFinishedAction *axisValueFinishedAction = new AMAxisValueFinishedAction(axisValueFinishedInfo);
			regionList->addSubAction(axisValueFinishedAction);
		}
	}

	// Otherwise, we need a detector dwell set action and a control move position for each point in the region.
	else {

		int extendedPoints = int(round(( ((double)exafsRegion->regionEnd()) - ((double)exafsRegion->regionStart()) )/ ((double)exafsRegion->regionStep()) ));
		QVector<double> energyPositions = QVector<double>(extendedPoints);
		AMEnergyToKSpaceCalculator kCalculator = AMEnergyToKSpaceCalculator(exafsRegion->edgeEnergy());
		kCalculator.energyValues(exafsRegion->regionStart(), exafsRegion->regionStep(), exafsRegion->regionEnd(), energyPositions.data());
		AMVariableIntegrationTime timeCalculator = AMVariableIntegrationTime(exafsRegion->equation(), exafsRegion->regionTime(), exafsRegion->maximumTime(), exafsRegion->regionStart(), exafsRegion->regionStep(), exafsRegion->regionEnd(), exafsRegion->a2());
		QVector<double> variableIntegrationTimes = QVector<double>(extendedPoints);
		timeCalculator.variableTime(variableIntegrationTimes.data());

		for (int i = 0; i < extendedPoints; i++){

			AMListAction3 *detectorSetDwellList = new AMListAction3(new AMListActionInfo3(QString("Set All Detectors Dwell Times"), QString("Set %1 Detectors").arg(detectors_->count())), AMListAction3::Parallel);

			for(int x = 0; x < detectors_->count(); x++){

				AMAction3 *detectorSetDwellAction = detectors_->at(x)->createSetAcquisitionTimeAction(variableIntegrationTimes.at(i));

				if(detectorSetDwellAction)
					detectorSetDwellList->addSubAction(detectorSetDwellAction);

			}

			AMAction3 *controlMove = AMActionSupport::buildControlMoveAction(axisControl, energyPositions.at(i), false);
			controlMove->setGenerateScanActionMessage(true);
			regionList->addSubAction(controlMove);
			regionList->addSubAction(detectorSetDwellList);
			AMListAction3 *nextLevelHolderAction = new AMListAction3(new AMListActionInfo3("Holder Action for the Next Sublevel", "Holder Action for the Next Sublevel"));
			regionList->addSubAction(nextLevelHolderAction);
			AMAxisValueFinishedActionInfo *axisValueFinishedInfo = new AMAxisValueFinishedActionInfo;
			axisValueFinishedInfo->setShortDescription(QString("%1 axis value finshed").arg(exafsRegion->name()));
			AMAxisValueFinishedAction *axisValueFinishedAction = new AMAxisValueFinishedAction(axisValueFinishedInfo);
			regionList->addSubAction(axisValueFinishedAction);
		}

		if (isFinalRegion){

			AMListAction3 *detectorSetDwellList = new AMListAction3(new AMListActionInfo3(QString("Set All Detectors Dwell Times"), QString("Set %1 Detectors").arg(detectors_->count())), AMListAction3::Parallel);

			for(int x = 0; x < detectors_->count(); x++){

				AMAction3 *detectorSetDwellAction = detectors_->at(x)->createSetAcquisitionTimeAction(double(exafsRegion->maximumTime()));

				if(detectorSetDwellAction)
					detectorSetDwellList->addSubAction(detectorSetDwellAction);
			}

			AMAction3 *controlMove = AMActionSupport::buildControlMoveAction(axisControl, double(kCalculator.energy(exafsRegion->regionEnd())), false);
			controlMove->setGenerateScanActionMessage(true);
			regionList->addSubAction(controlMove);
			regionList->addSubAction(detectorSetDwellList);
			AMListAction3 *nextLevelHolderAction = new AMListAction3(new AMListActionInfo3("Holder Action for the Next Sublevel", "Holder Action for the Next Sublevel"));
			regionList->addSubAction(nextLevelHolderAction);
			AMAxisValueFinishedActionInfo *axisValueFinishedInfo = new AMAxisValueFinishedActionInfo;
			axisValueFinishedInfo->setShortDescription(QString("%1 axis value finshed").arg(exafsRegion->name()));
			AMAxisValueFinishedAction *axisValueFinishedAction = new AMAxisValueFinishedAction(axisValueFinishedInfo);
			regionList->addSubAction(axisValueFinishedAction);
		}
	}


	return regionList;
}
