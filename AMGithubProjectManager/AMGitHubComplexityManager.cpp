#include "AMGitHubComplexityManager.h"

AMGitHubComplexityManager::AMGitHubComplexityManager(QObject *parent) :
	QObject(parent)
{
//	complexityMappingMatrix_ = QVector<int>(31, 0);
	int maxEstimate = int(AMGitHubIssue::EstimatedComplexityInvalid)-1;
	int maxActual = int(AMGitHubIssue::ActualComplexityInvalid)-1;
	complexityMappingMatrix_ = QVector<QList<AMGitHubIssue*> >(maxEstimate*maxActual);
}

double AMGitHubComplexityManager::probabilityOfMapping(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue, AMGitHubIssue::ActualComplexityValue actualComplexityValue) const
{
	AMGitHubComplexityMapping *tempMapping = new AMGitHubComplexityMapping(actualComplexityValue, estimatedComplexityValue);
	double retVal = probabilityOfMapping(tempMapping);
	tempMapping->deleteLater();
	return retVal;
}

double AMGitHubComplexityManager::probabilityOfMapping(AMGitHubComplexityMapping *complexityMapping) const
{
	if(!complexityMapping->validMapping())
		return 0;

	double matrixCount = complexityMappingMatrix_.at(complexityMapping->mappingIndex()).count();
	double totalCount = mappedIssues_.count();

	if(totalCount == 0)
		return 0;
	return matrixCount/totalCount;
}

double AMGitHubComplexityManager::probabilityOfMappingInColumn(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue, AMGitHubIssue::ActualComplexityValue actualComplexityValue) const
{
	AMGitHubComplexityMapping *tempMapping = new AMGitHubComplexityMapping(actualComplexityValue, estimatedComplexityValue);
	double retVal = probabilityOfMappingInColumn(tempMapping);
	tempMapping->deleteLater();
	return retVal;
}

double AMGitHubComplexityManager::probabilityOfMappingInColumn(AMGitHubComplexityMapping *complexityMapping) const
{
	if(!complexityMapping->validMapping())
		return 0;

	double matrixCount = complexityMappingMatrix_.at(complexityMapping->mappingIndex()).count();
	double columnCount = 0;

	int estimatedAsInt = int(complexityMapping->estimatedComplexityValue());
	int maxEstimate = int(AMGitHubIssue::EstimatedComplexityInvalid)-1;
	for(int x = 0, size = complexityMappingMatrix_.size(); x < size; x++){
		if(x%maxEstimate == estimatedAsInt)
			columnCount += complexityMappingMatrix_.at(x).count();
	}

	if(columnCount == 0)
		return 0;
	return matrixCount/columnCount;
}

double AMGitHubComplexityManager::averageTimeForEstimatedComplexity(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue) const
{
	int estimatedAsInt = int(estimatedComplexityValue);
	int maxEstimate = int(AMGitHubIssue::EstimatedComplexityInvalid)-1;

	double runningTotal = 0;
	int totalComplexityValues = 0;

	for(int x = 0, xSize = complexityMappingMatrix_.size(); x < xSize; x++){
		if(x%maxEstimate == estimatedAsInt){
			QList<AMGitHubIssue*> oneList = complexityMappingMatrix_[x];
			for(int y = 0, ySize = oneList.count(); y < ySize; y++){
				if(oneList.at(y)->normalizedTimeEstimate() > 0){
					totalComplexityValues++;
					runningTotal += oneList.at(y)->normalizedTimeEstimate();
				}
			}
		}
	}

	if(totalComplexityValues == 0)
		return 0;
	double retVal = double(runningTotal/(double)totalComplexityValues);
	return retVal;
}

double AMGitHubComplexityManager::averageTimeForActualComplexity(AMGitHubIssue::ActualComplexityValue actualComplexityValue) const
{
	int actualAsInt = int(actualComplexityValue);
	int maxEstimate = int(AMGitHubIssue::EstimatedComplexityInvalid)-1;

	double runningTotal = 0;
	int totalComplexityValues = 0;

	for(int x = 0, xSize = complexityMappingMatrix_.size(); x < xSize; x++){
		if(x/maxEstimate == actualAsInt){
			QList<AMGitHubIssue*> oneList = complexityMappingMatrix_[x];
			for(int y = 0, ySize = oneList.count(); y < ySize; y++){
				if(oneList.at(y)->normalizedTimeEstimate() > 0){
					totalComplexityValues++;
					runningTotal += oneList.at(y)->normalizedTimeEstimate();
				}
			}
		}
	}

	if(totalComplexityValues == 0)
		return 0;
	double retVal = double(runningTotal/(double)totalComplexityValues);
	return retVal;
}

double AMGitHubComplexityManager::probableTimeForEstimatedComplexity(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue) const
{
	double runningTotal = 0;

	int maxActual = int(AMGitHubIssue::ActualComplexityInvalid)-1;
	for(int x = 0, size = maxActual; x <size; x++){
		AMGitHubIssue::ActualComplexityValue oneActualComplexity = AMGitHubIssue::ActualComplexityValue(x);
		runningTotal += probabilityOfMappingInColumn(estimatedComplexityValue, oneActualComplexity)*averageTimeForActualComplexity(oneActualComplexity);
	}

	return runningTotal;
}

QString AMGitHubComplexityManager::probableTimeStringForEstimatedComplexity(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue) const
{
	QString retVal;
	double runningTotal = 0;

	int maxActual = int(AMGitHubIssue::ActualComplexityInvalid)-1;
	double estatimatedActualInt = AMGitHubIssue::integerFromEstimatedComplexity(estimatedComplexityValue);
	for(int x = 0, size = maxActual; x <size; x++){
		AMGitHubIssue::ActualComplexityValue oneActualComplexity = AMGitHubIssue::ActualComplexityValue(x);
		runningTotal += probabilityOfMappingInColumn(estimatedComplexityValue, oneActualComplexity)*averageTimeForActualComplexity(oneActualComplexity);
		double actualActualInt = AMGitHubIssue::integerFromActualComplexity(oneActualComplexity);
		retVal.append(QString("[%1-->%2]%3*%4 +").arg(estatimatedActualInt, 0, 'f', 0).arg(actualActualInt, 0, 'f', 0).arg(probabilityOfMappingInColumn(estimatedComplexityValue, oneActualComplexity), 0, 'f', 3).arg(averageTimeForActualComplexity(oneActualComplexity), 0, 'f', 3));
	}
	retVal.remove(retVal.size()-2, 2);
	retVal.prepend(QString("Estimate %1 as %2: ").arg(estatimatedActualInt, 0, 'f', 0).arg(runningTotal, 0, 'f', 3));
	return retVal;
}

QString AMGitHubComplexityManager::fullMatrixString() const
{
	QString retVal;

	int maxEstimate = int(AMGitHubIssue::EstimatedComplexityInvalid)-1;
	int maxActual = int(AMGitHubIssue::ActualComplexityInvalid)-1;

	retVal.append(QString("------ "));
	for(int x = 0, size = maxActual; x <= size; x++){
		AMGitHubIssue::ActualComplexityValue oneActual = AMGitHubIssue::ActualComplexityValue(x);
		double actualInt = AMGitHubIssue::integerFromActualComplexity(oneActual);
		retVal.append(QString("%1").arg(actualInt, -7, 'f', 0, ' '));
	}
	retVal.append("\n");

	double runningTotal = 0;
	int rowCounter = 0;
	AMGitHubIssue::EstimatedComplexityValue oneEstimated = AMGitHubIssue::EstimatedComplexityValue(rowCounter);
	double estimatedInt = AMGitHubIssue::integerFromEstimatedComplexity(oneEstimated);
	retVal.append(QString("%1").arg(estimatedInt, -7, 'f', 0, ' '));
	for(int x = 0, size = complexityMappingMatrix_.count(); x < size; x++){
		AMGitHubComplexityMapping *tempMapping = new AMGitHubComplexityMapping(x);
		retVal.append(QString("%1").arg(probabilityOfMapping(tempMapping), -7, 'f', 3, ' '));
		runningTotal += probabilityOfMapping(tempMapping);
		if(x%maxEstimate == maxEstimate-1){
			retVal.append("\n");
			rowCounter++;
			AMGitHubIssue::EstimatedComplexityValue oneEstimated = AMGitHubIssue::EstimatedComplexityValue(rowCounter);
			double estimatedInt = AMGitHubIssue::integerFromEstimatedComplexity(oneEstimated);
			retVal.append(QString("%1").arg(estimatedInt, -7, 'f', 0, ' '));
		}
	}
	retVal.append("\n");
	retVal.append(QString("Cumulative: %1").arg(runningTotal, 0, 'f', 3));
	return retVal;
}

//double AMGitHubComplexityManager::averageTimeForEstimatedComplexity(AMGitHubIssue::ActualComplexityValue estimatedComplexityValue){
//	QList<double> localEstimatedComplexityTime;
//	switch(estimatedComplexityValue){
//	case AMGitHubIssue::ActualComplexity1:
//		localEstimatedComplexityTime = estimatedComplexity1Times_;
//		break;
//	case AMGitHubIssue::ActualComplexity2:
//		localEstimatedComplexityTime = estimatedComplexity2Times_;
//		break;
//	case AMGitHubIssue::ActualComplexity3:
//		localEstimatedComplexityTime = estimatedComplexity3Times_;
//		break;
//	case AMGitHubIssue::ActualComplexity5:
//		localEstimatedComplexityTime = estimatedComplexity5Times_;
//		break;
//	case AMGitHubIssue::ActualComplexity8:
//		localEstimatedComplexityTime = estimatedComplexity8Times_;
//		break;
////	case AMGitHubIssue::ComplexityK:
////		localEstimatedComplexityTime = estimatedComplexityKTimes_;
////		break;
//	case AMGitHubIssue::ActualComplexityInvalid:
//		break;
//	}

//	if(localEstimatedComplexityTime.count() == 0)
//		return 0;

//	double retVal = 0;
//	for(int x = 0, size = localEstimatedComplexityTime.count(); x < size; x++)
//		retVal += localEstimatedComplexityTime.at(x);
//	retVal = retVal/localEstimatedComplexityTime.count();
//	return retVal;
//}

//double AMGitHubComplexityManager::averageTimeForActualComplexity(AMGitHubIssue::ActualComplexityValue actualComplexityValue){
//	QList<double> localActualComplexityTime;
//	switch(actualComplexityValue){
//	case AMGitHubIssue::ActualComplexity1:
//		localActualComplexityTime = actualComplexity1Times_;
//		break;
//	case AMGitHubIssue::ActualComplexity2:
//		localActualComplexityTime = actualComplexity2Times_;
//		break;
//	case AMGitHubIssue::ActualComplexity3:
//		localActualComplexityTime = actualComplexity3Times_;
//		break;
//	case AMGitHubIssue::ActualComplexity5:
//		localActualComplexityTime = actualComplexity5Times_;
//		break;
//	case AMGitHubIssue::ActualComplexity8:
//		localActualComplexityTime = actualComplexity8Times_;
//		break;
////	case AMGitHubIssue::ComplexityK:
////		return 0;
////		localActualComplexityTime = actualComplexityKTimes_;
////		break;
//	case AMGitHubIssue::ActualComplexityInvalid:
//		return 0;
//	}

//	if(localActualComplexityTime.count() == 0)
//		return 0;

//	double retVal = 0;
//	for(int x = 0, size = localActualComplexityTime.count(); x < size; x++)
//		retVal += localActualComplexityTime.at(x);
//	retVal = retVal/localActualComplexityTime.count();
//	return retVal;
//}

//double AMGitHubComplexityManager::probableTimeForEstimatedComplexity(AMGitHubIssue::ActualComplexityValue estimatedComplexityValue){
//	int localTotalEstimatedComplexityIssues = 0;
//	int indexStartValue;

//	switch(estimatedComplexityValue){
//	case AMGitHubIssue::ActualComplexity1:
//		indexStartValue = 1;
//		break;
//	case AMGitHubIssue::ActualComplexity2:
//		indexStartValue = 6;
//		break;
//	case AMGitHubIssue::ActualComplexity3:
//		indexStartValue = 11;
//		break;
//	case AMGitHubIssue::ActualComplexity5:
//		indexStartValue = 16;
//		break;
//	case AMGitHubIssue::ActualComplexity8:
//		indexStartValue = 21;
//		break;
////	case AMGitHubIssue::ComplexityK:
////		return -1;
////		indexStartValue = 26;
////		break;
//	case AMGitHubIssue::ActualComplexityInvalid:
//		return 0;
//	}

//	for(int x = indexStartValue, size = indexStartValue+5; x < size; x++)
//		localTotalEstimatedComplexityIssues += complexityMappingMatrix_.at(x);

//	double retVal = 0;
//	retVal = averageTimeForActualComplexity(AMGitHubIssue::ActualComplexity1)*complexityMappingMatrix_.at(indexStartValue)/localTotalEstimatedComplexityIssues
//			+ averageTimeForActualComplexity(AMGitHubIssue::ActualComplexity2)*complexityMappingMatrix_.at(indexStartValue+1)/localTotalEstimatedComplexityIssues
//			+ averageTimeForActualComplexity(AMGitHubIssue::ActualComplexity3)*complexityMappingMatrix_.at(indexStartValue+2)/localTotalEstimatedComplexityIssues
//			+ averageTimeForActualComplexity(AMGitHubIssue::ActualComplexity5)*complexityMappingMatrix_.at(indexStartValue+3)/localTotalEstimatedComplexityIssues
//			+ averageTimeForActualComplexity(AMGitHubIssue::ActualComplexity8)*complexityMappingMatrix_.at(indexStartValue+4)/localTotalEstimatedComplexityIssues;

//	return retVal;
//}

//double AMGitHubComplexityManager::outstandingEstimateAtDate(AMGitHubIssueFamily *issueFamily, const QDateTime &date){
//	if(!issueFamily || !issueFamily->originatingIssue() || !issueFamily->originatingIssue()->createdDate().isValid())
//		return 0;

//	AMGitHubIssue *originatingIssue = issueFamily->originatingIssue();
//	if(date < originatingIssue->createdDate())
//		return 0;

//	if(originatingIssue->closedDate().isValid() && (date >= originatingIssue->closedDate()) )
//		return 0;

//	return probableTimeForEstimatedComplexity(issueFamily->estimatedComplexity());
//}

//double AMGitHubComplexityManager::completedEstimateAtDate(AMGitHubIssueFamily *issueFamily, const QDateTime &date){
//	if(!issueFamily || !issueFamily->originatingIssue() || !issueFamily->originatingIssue()->createdDate().isValid())
//		return 0;

//	AMGitHubIssue *originatingIssue = issueFamily->originatingIssue();
//	if(date < originatingIssue->createdDate())
//		return 0;

////	if(originatingIssue->closedDate().isValid() && (date >= originatingIssue->closedDate()) && (issueFamily->actualComplexity() != AMGitHubIssue::InvalidComplexity) )
////		return averageTimeForActualComplexity(issueFamily->actualComplexity());

//	if(originatingIssue->closedDate().isValid() && (date >= originatingIssue->closedDate()) )
//		return probableTimeForEstimatedComplexity(issueFamily->estimatedComplexity());

//	return 0;
//}

//double AMGitHubComplexityManager::withdrawnEstimateAtDate(AMGitHubIssueFamily *issueFamily, const QDateTime &date){
//	if(!issueFamily || !issueFamily->originatingIssue() || !issueFamily->originatingIssue()->createdDate().isValid())
//		return 0;

//	AMGitHubIssue *originatingIssue = issueFamily->originatingIssue();
//	if(date < originatingIssue->createdDate())
//		return 0;

//	if(originatingIssue->closedDate().isValid() && (date >= originatingIssue->closedDate()) && !issueFamily->pullRequestIssue() && !originatingIssue->inlineIssue() )
//		return probableTimeForEstimatedComplexity(issueFamily->estimatedComplexity());

//	return 0;
//}

//double AMGitHubComplexityManager::complexityMappedCompletedWorkAtDate(AMGitHubIssueFamily *issueFamily, const QDateTime &date){
//	if(!issueFamily || !issueFamily->originatingIssue() || !issueFamily->originatingIssue()->createdDate().isValid())
//		return 0;

//	AMGitHubIssue *originatingIssue = issueFamily->originatingIssue();
//	if(date < originatingIssue->createdDate())
//		return 0;

//	if(originatingIssue->closedDate().isValid() && (date >= originatingIssue->closedDate()) && (issueFamily->actualComplexity() != AMGitHubIssue::ActualComplexityInvalid) )
//		return averageTimeForActualComplexity(issueFamily->actualComplexity());

//	return 0;
//}

//double AMGitHubComplexityManager::reportedCompletedWorkAtDate(AMGitHubIssueFamily *issueFamily, const QDateTime &date){
//	if(!issueFamily || !issueFamily->originatingIssue() || !issueFamily->originatingIssue()->createdDate().isValid())
//		return 0;

//	AMGitHubIssue *originatingIssue = issueFamily->originatingIssue();
//	if(date < originatingIssue->createdDate())
//		return 0;

//	if(originatingIssue->closedDate().isValid() && (date >= originatingIssue->closedDate()) && (issueFamily->actualComplexity() != AMGitHubIssue::ActualComplexityInvalid) ){
//		if(issueFamily->normalizedTimeEstiamte() < 0)
//			return 0;
//		return issueFamily->normalizedTimeEstiamte();
//	}

//	return 0;
//}

//void AMGitHubComplexityManager::incrementComplexityMapping(AMGitHubIssueFamily::ComplexityMapping complexityMapping){
//	complexityMappingMatrix_[complexityMapping] = complexityMappingMatrix_.at(complexityMapping)+1;
//}

bool AMGitHubComplexityManager::addMapping(AMGitHubIssue *oneIssue)
{
	if(!oneIssue)
		return false;
	if(mappedIssues_.contains(oneIssue->issueNumber()))
		return false;
	if(!oneIssue->complexityMapping()->validMapping())
		return false;

	mappedIssues_.insert(oneIssue->issueNumber(), oneIssue);
	complexityMappingMatrix_[oneIssue->complexityMapping()->mappingIndex()].append(oneIssue);
	return true;
}

void AMGitHubComplexityManager::clearComplexityMappings(){

	int maxEstimate = int(AMGitHubIssue::EstimatedComplexityInvalid)-1;
	int maxActual = int(AMGitHubIssue::ActualComplexityInvalid)-1;
	complexityMappingMatrix_ = QVector<QList<AMGitHubIssue*> >(maxEstimate*maxActual);
	mappedIssues_.clear();
}

//void AMGitHubComplexityManager::addTimeEstimateForEstimatedComplexity(double normalizedTimeEstimate, AMGitHubIssue::ActualComplexityValue complexityValue){
//	if(normalizedTimeEstimate <= 0)
//		return;

//	switch(complexityValue){
//	case AMGitHubIssue::ActualComplexity1:
//		estimatedComplexity1Times_.append(normalizedTimeEstimate);
//		break;
//	case AMGitHubIssue::ActualComplexity2:
//		estimatedComplexity2Times_.append(normalizedTimeEstimate);
//		break;
//	case AMGitHubIssue::ActualComplexity3:
//		estimatedComplexity3Times_.append(normalizedTimeEstimate);
//		break;
//	case AMGitHubIssue::ActualComplexity5:
//		estimatedComplexity5Times_.append(normalizedTimeEstimate);
//		break;
//	case AMGitHubIssue::ActualComplexity8:
//		estimatedComplexity8Times_.append(normalizedTimeEstimate);
//		break;
////	case AMGitHubIssue::ComplexityK:
////		estimatedComplexityKTimes_.append(normalizedTimeEstimate);
////		break;
//	case AMGitHubIssue::ActualComplexityInvalid:
//		break;
//	}
//}

//void AMGitHubComplexityManager::addTimeEstimateForActualComplexity(double normalizedTimeEstimate, AMGitHubIssue::ActualComplexityValue complexityValue){
//	if(normalizedTimeEstimate <= 0)
//		return;

//	switch(complexityValue){
//	case AMGitHubIssue::ActualComplexity1:
//		actualComplexity1Times_.append(normalizedTimeEstimate);
//		break;
//	case AMGitHubIssue::ActualComplexity2:
//		actualComplexity2Times_.append(normalizedTimeEstimate);
//		break;
//	case AMGitHubIssue::ActualComplexity3:
//		actualComplexity3Times_.append(normalizedTimeEstimate);
//		break;
//	case AMGitHubIssue::ActualComplexity5:
//		actualComplexity5Times_.append(normalizedTimeEstimate);
//		break;
//	case AMGitHubIssue::ActualComplexity8:
//		actualComplexity8Times_.append(normalizedTimeEstimate);
//		break;
////	case AMGitHubIssue::ComplexityK:
//////		actualComplexityKTimes_.append(normalizedTimeEstimate);
////		break;
//	case AMGitHubIssue::ActualComplexityInvalid:
//		break;
//	}
//}
