#include "AMGitHubComplexityManager.h"

AMGitHubComplexityManager::AMGitHubComplexityManager(QObject *parent) :
	QObject(parent)
{
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

double AMGitHubComplexityManager::outstandingEstimateAtDate(AMGitHubIssue *issue, const QDateTime &date)
{
	if(!issue || !issue->createdDate().isValid())
		return 0;

	if(date < issue->createdDate())
		return 0;

	if(issue->closedDate().isValid() && (date >= issue->closedDate()) )
		return 0;

	return probableTimeForEstimatedComplexity(issue->estimatedComplexityValue());
}

double AMGitHubComplexityManager::completedEstimateAtDate(AMGitHubIssue *issue, const QDateTime &date)
{
	if(!issue || !issue->createdDate().isValid())
		return 0;

	if(date < issue->createdDate())
		return 0;

	if(issue->closedDate().isValid() && (date >= issue->closedDate()) )
		return probableTimeForEstimatedComplexity(issue->estimatedComplexityValue());

	return 0;
}

double AMGitHubComplexityManager::withdrawnEstimateAtDate(AMGitHubIssue *issue, const QDateTime &date){
	if(!issue || !issue->createdDate().isValid())
		return 0;

	if(date < issue->createdDate())
		return 0;

	if(issue->closedDate().isValid() && (date >= issue->closedDate()) && (issue->actualComplexityValue() == AMGitHubIssue::ActualComplexityInvalid))
		return probableTimeForEstimatedComplexity(issue->estimatedComplexityValue());

	return 0;
}

double AMGitHubComplexityManager::complexityMappedCompletedWorkAtDate(AMGitHubIssue *issue, const QDateTime &date){
	if(!issue || !issue->createdDate().isValid())
		return 0;

	if(date < issue->createdDate())
		return 0;

	if(issue->closedDate().isValid() && (date >= issue->closedDate()) && (issue->actualComplexityValue() != AMGitHubIssue::ActualComplexityInvalid) )
		return averageTimeForActualComplexity(issue->actualComplexityValue());

	return 0;
}

double AMGitHubComplexityManager::reportedCompletedWorkAtDate(AMGitHubIssue *issue, const QDateTime &date){
	if(!issue || !issue->createdDate().isValid())
		return 0;

	if(date < issue->createdDate())
		return 0;

	if(issue->closedDate().isValid() && (date >= issue->closedDate()) && (issue->actualComplexityValue() != AMGitHubIssue::ActualComplexityInvalid) ){
		if(issue->normalizedTimeEstimate() < 0)
			return 0;
		return issue->normalizedTimeEstimate();
	}

	return 0;
}

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
