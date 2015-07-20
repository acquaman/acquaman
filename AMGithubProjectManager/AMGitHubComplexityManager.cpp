#include "AMGitHubComplexityManager.h"

AMGitHubComplexityManager::AMGitHubComplexityManager(QObject *parent) :
	QObject(parent)
{
	complexityMappingMatrix_ = QVector<int>(31, 0);
}

double AMGitHubComplexityManager::averageTimeForEstimatedComplexity(AMGitHubIssue::ComplexityValue estimatedComplexityValue){
	QList<double> localEstimatedComplexityTime;
	switch(estimatedComplexityValue){
	case AMGitHubIssue::Complexity1:
		localEstimatedComplexityTime = estimatedComplexity1Times_;
		break;
	case AMGitHubIssue::Complexity2:
		localEstimatedComplexityTime = estimatedComplexity2Times_;
		break;
	case AMGitHubIssue::Complexity3:
		localEstimatedComplexityTime = estimatedComplexity3Times_;
		break;
	case AMGitHubIssue::Complexity5:
		localEstimatedComplexityTime = estimatedComplexity5Times_;
		break;
	case AMGitHubIssue::Complexity8:
		localEstimatedComplexityTime = estimatedComplexity8Times_;
		break;
	case AMGitHubIssue::ComplexityK:
		localEstimatedComplexityTime = estimatedComplexityKTimes_;
		break;
	case AMGitHubIssue::InvalidComplexity:
		break;
	}

	if(localEstimatedComplexityTime.count() == 0)
		return 0;

	double retVal = 0;
	for(int x = 0, size = localEstimatedComplexityTime.count(); x < size; x++)
		retVal += localEstimatedComplexityTime.at(x);
	retVal = retVal/localEstimatedComplexityTime.count();
	return retVal;
}

double AMGitHubComplexityManager::averageTimeForActualComplexity(AMGitHubIssue::ComplexityValue actualComplexityValue){
	QList<double> localActualComplexityTime;
	switch(actualComplexityValue){
	case AMGitHubIssue::Complexity1:
		localActualComplexityTime = actualComplexity1Times_;
		break;
	case AMGitHubIssue::Complexity2:
		localActualComplexityTime = actualComplexity2Times_;
		break;
	case AMGitHubIssue::Complexity3:
		localActualComplexityTime = actualComplexity3Times_;
		break;
	case AMGitHubIssue::Complexity5:
		localActualComplexityTime = actualComplexity5Times_;
		break;
	case AMGitHubIssue::Complexity8:
		localActualComplexityTime = actualComplexity8Times_;
		break;
	case AMGitHubIssue::ComplexityK:
		return 0;
//		localActualComplexityTime = actualComplexityKTimes_;
//		break;
	case AMGitHubIssue::InvalidComplexity:
		return 0;
	}

	if(localActualComplexityTime.count() == 0)
		return 0;

	double retVal = 0;
	for(int x = 0, size = localActualComplexityTime.count(); x < size; x++)
		retVal += localActualComplexityTime.at(x);
	retVal = retVal/localActualComplexityTime.count();
	return retVal;
}

double AMGitHubComplexityManager::probableTimeForEstimatedComplexity(AMGitHubIssue::ComplexityValue estimatedComplexityValue){
	int localTotalEstimatedComplexityIssues = 0;
	int indexStartValue;

	switch(estimatedComplexityValue){
	case AMGitHubIssue::Complexity1:
		indexStartValue = 1;
		break;
	case AMGitHubIssue::Complexity2:
		indexStartValue = 6;
		break;
	case AMGitHubIssue::Complexity3:
		indexStartValue = 11;
		break;
	case AMGitHubIssue::Complexity5:
		indexStartValue = 16;
		break;
	case AMGitHubIssue::Complexity8:
		indexStartValue = 21;
		break;
	case AMGitHubIssue::ComplexityK:
		return -1;
//		indexStartValue = 26;
//		break;
	case AMGitHubIssue::InvalidComplexity:
		return 0;
	}

	for(int x = indexStartValue, size = indexStartValue+5; x < size; x++)
		localTotalEstimatedComplexityIssues += complexityMappingMatrix_.at(x);

	double retVal = 0;
	retVal = averageTimeForActualComplexity(AMGitHubIssue::Complexity1)*complexityMappingMatrix_.at(indexStartValue)/localTotalEstimatedComplexityIssues
			+ averageTimeForActualComplexity(AMGitHubIssue::Complexity2)*complexityMappingMatrix_.at(indexStartValue+1)/localTotalEstimatedComplexityIssues
			+ averageTimeForActualComplexity(AMGitHubIssue::Complexity3)*complexityMappingMatrix_.at(indexStartValue+2)/localTotalEstimatedComplexityIssues
			+ averageTimeForActualComplexity(AMGitHubIssue::Complexity5)*complexityMappingMatrix_.at(indexStartValue+3)/localTotalEstimatedComplexityIssues
			+ averageTimeForActualComplexity(AMGitHubIssue::Complexity8)*complexityMappingMatrix_.at(indexStartValue+4)/localTotalEstimatedComplexityIssues;

	return retVal;
}

double AMGitHubComplexityManager::outstandingEstimateAtDate(AMGitHubIssueFamily *issueFamily, const QDateTime &date){
	if(!issueFamily || !issueFamily->originatingIssue() || !issueFamily->originatingIssue()->createdDate().isValid())
		return 0;

	AMGitHubIssue *originatingIssue = issueFamily->originatingIssue();
	if(date < originatingIssue->createdDate())
		return 0;

	if(originatingIssue->closedDate().isValid() && (date >= originatingIssue->closedDate()) )
		return 0;

	return probableTimeForEstimatedComplexity(issueFamily->estimatedComplexity());
}

double AMGitHubComplexityManager::completedEstimateAtDate(AMGitHubIssueFamily *issueFamily, const QDateTime &date){
	if(!issueFamily || !issueFamily->originatingIssue() || !issueFamily->originatingIssue()->createdDate().isValid())
		return 0;

	AMGitHubIssue *originatingIssue = issueFamily->originatingIssue();
	if(date < originatingIssue->createdDate())
		return 0;

//	if(originatingIssue->closedDate().isValid() && (date >= originatingIssue->closedDate()) && (issueFamily->actualComplexity() != AMGitHubIssue::InvalidComplexity) )
//		return averageTimeForActualComplexity(issueFamily->actualComplexity());

	if(originatingIssue->closedDate().isValid() && (date >= originatingIssue->closedDate()) )
		return probableTimeForEstimatedComplexity(issueFamily->estimatedComplexity());

	return 0;
}

double AMGitHubComplexityManager::withdrawnEstimateAtDate(AMGitHubIssueFamily *issueFamily, const QDateTime &date){
	if(!issueFamily || !issueFamily->originatingIssue() || !issueFamily->originatingIssue()->createdDate().isValid())
		return 0;

	AMGitHubIssue *originatingIssue = issueFamily->originatingIssue();
	if(date < originatingIssue->createdDate())
		return 0;

	if(originatingIssue->closedDate().isValid() && (date >= originatingIssue->closedDate()) && !issueFamily->pullRequestIssue() && !originatingIssue->inlineIssue() )
		return probableTimeForEstimatedComplexity(issueFamily->estimatedComplexity());

	return 0;
}

double AMGitHubComplexityManager::complexityMappedCompletedWorkAtDate(AMGitHubIssueFamily *issueFamily, const QDateTime &date){
	if(!issueFamily || !issueFamily->originatingIssue() || !issueFamily->originatingIssue()->createdDate().isValid())
		return 0;

	AMGitHubIssue *originatingIssue = issueFamily->originatingIssue();
	if(date < originatingIssue->createdDate())
		return 0;

	if(originatingIssue->closedDate().isValid() && (date >= originatingIssue->closedDate()) && (issueFamily->actualComplexity() != AMGitHubIssue::InvalidComplexity) )
		return averageTimeForActualComplexity(issueFamily->actualComplexity());

	return 0;
}

double AMGitHubComplexityManager::reportedCompletedWorkAtDate(AMGitHubIssueFamily *issueFamily, const QDateTime &date){
	if(!issueFamily || !issueFamily->originatingIssue() || !issueFamily->originatingIssue()->createdDate().isValid())
		return 0;

	AMGitHubIssue *originatingIssue = issueFamily->originatingIssue();
	if(date < originatingIssue->createdDate())
		return 0;

	if(originatingIssue->closedDate().isValid() && (date >= originatingIssue->closedDate()) && (issueFamily->actualComplexity() != AMGitHubIssue::InvalidComplexity) ){
		if(issueFamily->normalizedTimeEstiamte() < 0)
			return 0;
		return issueFamily->normalizedTimeEstiamte();
	}

	return 0;
}

void AMGitHubComplexityManager::incrementComplexityMapping(AMGitHubIssueFamily::ComplexityMapping complexityMapping){
	complexityMappingMatrix_[complexityMapping] = complexityMappingMatrix_.at(complexityMapping)+1;
}

void AMGitHubComplexityManager::clearComplexityMappings(){
	complexityMappingMatrix_ = QVector<int>(31, 0);;
}

void AMGitHubComplexityManager::addTimeEstimateForEstimatedComplexity(double normalizedTimeEstimate, AMGitHubIssue::ComplexityValue complexityValue){
	if(normalizedTimeEstimate <= 0)
		return;

	switch(complexityValue){
	case AMGitHubIssue::Complexity1:
		estimatedComplexity1Times_.append(normalizedTimeEstimate);
		break;
	case AMGitHubIssue::Complexity2:
		estimatedComplexity2Times_.append(normalizedTimeEstimate);
		break;
	case AMGitHubIssue::Complexity3:
		estimatedComplexity3Times_.append(normalizedTimeEstimate);
		break;
	case AMGitHubIssue::Complexity5:
		estimatedComplexity5Times_.append(normalizedTimeEstimate);
		break;
	case AMGitHubIssue::Complexity8:
		estimatedComplexity8Times_.append(normalizedTimeEstimate);
		break;
	case AMGitHubIssue::ComplexityK:
		estimatedComplexityKTimes_.append(normalizedTimeEstimate);
		break;
	case AMGitHubIssue::InvalidComplexity:
		break;
	}
}

void AMGitHubComplexityManager::addTimeEstimateForActualComplexity(double normalizedTimeEstimate, AMGitHubIssue::ComplexityValue complexityValue){
	if(normalizedTimeEstimate <= 0)
		return;

	switch(complexityValue){
	case AMGitHubIssue::Complexity1:
		actualComplexity1Times_.append(normalizedTimeEstimate);
		break;
	case AMGitHubIssue::Complexity2:
		actualComplexity2Times_.append(normalizedTimeEstimate);
		break;
	case AMGitHubIssue::Complexity3:
		actualComplexity3Times_.append(normalizedTimeEstimate);
		break;
	case AMGitHubIssue::Complexity5:
		actualComplexity5Times_.append(normalizedTimeEstimate);
		break;
	case AMGitHubIssue::Complexity8:
		actualComplexity8Times_.append(normalizedTimeEstimate);
		break;
	case AMGitHubIssue::ComplexityK:
//		actualComplexityKTimes_.append(normalizedTimeEstimate);
		break;
	case AMGitHubIssue::InvalidComplexity:
		break;
	}
}
