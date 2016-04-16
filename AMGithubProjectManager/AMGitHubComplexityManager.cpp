#include "AMGitHubComplexityManager.h"

#include <QDebug>

#include <math.h>

double AMGitHubStats::mean(const QVector<double> &input)
{
	if(input.count() == 0)
		return 0;

	double retVal = 0;
	for(int x = 0, size = input.count(); x< size; x++)
		retVal += input.at(x);
	return retVal/(double(input.count()));
}

double AMGitHubStats::standardDeviation(const QVector<double> &input, double useMean)
{
	if(input.count() == 0)
		return 0;

	double inputMean;
	if(useMean > 0)
		inputMean = useMean;
	else
		inputMean = mean(input);

	QVector<double> deviations = QVector<double>(input.count());
	for(int x = 0, size = input.count(); x < size; x++)
		deviations[x] = pow(input.at(x)-inputMean, 2);

	double retVal = sqrt(mean(deviations));
	return retVal;
}

AMGitHubComplexityManager::AMGitHubComplexityManager(QObject *parent) :
	QObject(parent)
{
	int maxEstimate = int(AMGitHubIssue::EstimatedComplexityInvalid);
	int maxActual = int(AMGitHubIssue::ActualComplexityInvalid);
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

double AMGitHubComplexityManager::probabilityOfMappingInRow(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue, AMGitHubIssue::ActualComplexityValue actualComplexityValue) const
{
	AMGitHubComplexityMapping *tempMapping = new AMGitHubComplexityMapping(actualComplexityValue, estimatedComplexityValue);
	double retVal = probabilityOfMappingInRow(tempMapping);
	tempMapping->deleteLater();
	return retVal;
}

double AMGitHubComplexityManager::probabilityOfMappingInRow(AMGitHubComplexityMapping *complexityMapping) const
{
	if(!complexityMapping->validMapping())
		return 0;

	double matrixCount = complexityMappingMatrix_.at(complexityMapping->mappingIndex()).count();
	double rowCount = 0;

	int estimatedAsInt = int(complexityMapping->estimatedComplexityValue());
	int maxEstimate = int(AMGitHubIssue::EstimatedComplexityInvalid);
	for(int x = 0, size = complexityMappingMatrix_.size(); x < size; x++){
		if(x/maxEstimate == estimatedAsInt)
			rowCount += complexityMappingMatrix_.at(x).count();
	}

	if(rowCount == 0)
		return 0;
	return matrixCount/rowCount;
}

double AMGitHubComplexityManager::averageTimeForEstimatedComplexity(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue) const
{
	int estimatedAsInt = int(estimatedComplexityValue);
	int maxEstimate = int(AMGitHubIssue::EstimatedComplexityInvalid);
	QList<double> estimatedComplexityTimes;

	for(int x = 0, xSize = complexityMappingMatrix_.size(); x < xSize; x++){
		if(x/maxEstimate == estimatedAsInt){
			QList<AMGitHubIssue*> oneList = complexityMappingMatrix_[x];
			for(int y = 0, ySize = oneList.count(); y < ySize; y++){
				if(oneList.at(y)->normalizedTimeEstimate() > 0)
					estimatedComplexityTimes.append(oneList.at(y)->normalizedTimeEstimate());
			}
		}
	}

	double retVal = AMGitHubStats::mean(estimatedComplexityTimes.toVector());
	return retVal;
}

double AMGitHubComplexityManager::standardDeviationTimeForEstimatedComplexity(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue) const
{
	int estimatedAsInt = int(estimatedComplexityValue);
	int maxEstimate = int(AMGitHubIssue::EstimatedComplexityInvalid);
	QList<double> estimatedComplexityTimes;

	for(int x = 0, xSize = complexityMappingMatrix_.size(); x < xSize; x++){
		if(x/maxEstimate == estimatedAsInt){
			QList<AMGitHubIssue*> oneList = complexityMappingMatrix_[x];
			for(int y = 0, ySize = oneList.count(); y < ySize; y++){
				if(oneList.at(y)->normalizedTimeEstimate() > 0)
					estimatedComplexityTimes.append(oneList.at(y)->normalizedTimeEstimate());
			}
		}
	}

	double meanTime = averageTimeForEstimatedComplexity(estimatedComplexityValue);
	return AMGitHubStats::standardDeviation(estimatedComplexityTimes.toVector(), meanTime);
}

double AMGitHubComplexityManager::averageTimeForActualComplexity(AMGitHubIssue::ActualComplexityValue actualComplexityValue) const
{
	int actualAsInt = int(actualComplexityValue);
	int maxEstimate = int(AMGitHubIssue::EstimatedComplexityInvalid);
	QList<double> actualComplexityTimes;

	for(int x = 0, xSize = complexityMappingMatrix_.size(); x < xSize; x++){
		if(x%maxEstimate == actualAsInt){
			QList<AMGitHubIssue*> oneList = complexityMappingMatrix_[x];
			for(int y = 0, ySize = oneList.count(); y < ySize; y++){
				if(oneList.at(y)->normalizedTimeEstimate() > 0)
					actualComplexityTimes.append(oneList.at(y)->normalizedTimeEstimate());
			}
		}
	}

	double retVal = AMGitHubStats::mean(actualComplexityTimes.toVector());
	return retVal;
}

double AMGitHubComplexityManager::standardDeviationTimeForActualComplexity(AMGitHubIssue::ActualComplexityValue actualComplexityValue) const
{
	int actualAsInt = int(actualComplexityValue);
	int maxEstimate = int(AMGitHubIssue::EstimatedComplexityInvalid);
	QList<double> actualComplexityTimes;

	for(int x = 0, xSize = complexityMappingMatrix_.size(); x < xSize; x++){
		if(x%maxEstimate == actualAsInt){
			QList<AMGitHubIssue*> oneList = complexityMappingMatrix_[x];
			for(int y = 0, ySize = oneList.count(); y < ySize; y++){
				if(oneList.at(y)->normalizedTimeEstimate() > 0)
					actualComplexityTimes.append(oneList.at(y)->normalizedTimeEstimate());
			}
		}
	}

	double meanTime = averageTimeForActualComplexity(actualComplexityValue);
	return AMGitHubStats::standardDeviation(actualComplexityTimes.toVector(), meanTime);
}

double AMGitHubComplexityManager::probableTimeForEstimatedComplexity(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue) const
{
	double runningTotal = 0;

	int maxActual = int(AMGitHubIssue::ActualComplexityInvalid);
	for(int x = 0, size = maxActual; x < size; x++){
		AMGitHubIssue::ActualComplexityValue oneActualComplexity = AMGitHubIssue::ActualComplexityValue(x);
		runningTotal += probabilityOfMappingInRow(estimatedComplexityValue, oneActualComplexity)*averageTimeForActualComplexity(oneActualComplexity);
	}

	return runningTotal;
}

double AMGitHubComplexityManager::probablestandardDeviationTimeForEstimatedComplexity(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue) const
{
	double sumOfProportionalVariances = 0;

	int maxActual = int(AMGitHubIssue::ActualComplexityInvalid);
	for(int x = 0, size = maxActual; x < size; x++){
		AMGitHubIssue::ActualComplexityValue oneActualComplexity = AMGitHubIssue::ActualComplexityValue(x);
		sumOfProportionalVariances += probabilityOfMappingInRow(estimatedComplexityValue, oneActualComplexity)*pow(averageTimeForActualComplexity(oneActualComplexity), 2);
	}

	return sqrt(sumOfProportionalVariances);
}

QString AMGitHubComplexityManager::probableTimeStringForEstimatedComplexity(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue) const
{
	QString retVal;
	double runningTotal = 0;

	int maxActual = int(AMGitHubIssue::ActualComplexityInvalid);
	double estatimatedActualInt = AMGitHubIssue::integerFromEstimatedComplexity(estimatedComplexityValue);
	for(int x = 0, size = maxActual; x < size; x++){
		AMGitHubIssue::ActualComplexityValue oneActualComplexity = AMGitHubIssue::ActualComplexityValue(x);
		runningTotal += probabilityOfMappingInRow(estimatedComplexityValue, oneActualComplexity)*averageTimeForActualComplexity(oneActualComplexity);
		double actualActualInt = AMGitHubIssue::integerFromActualComplexity(oneActualComplexity);
		retVal.append(QString("[%1-->%2]%3*%4 +").arg(estatimatedActualInt, 0, 'f', 0).arg(actualActualInt, 0, 'f', 0).arg(probabilityOfMappingInRow(estimatedComplexityValue, oneActualComplexity), 0, 'f', 3).arg(averageTimeForActualComplexity(oneActualComplexity), 0, 'f', 3));
	}
	retVal.remove(retVal.size()-2, 2);
	retVal.prepend(QString("Estimate %1 as %2: ").arg(estatimatedActualInt, 0, 'f', 0).arg(runningTotal, 0, 'f', 3));
	return retVal;
}

QString AMGitHubComplexityManager::fullMatrixString() const
{
	QString indexProbabilityString;
	QString mappingProbabilityString;

	int maxEstimate = int(AMGitHubIssue::EstimatedComplexityInvalid);
	int maxActual = int(AMGitHubIssue::ActualComplexityInvalid);

	indexProbabilityString.append(QString("Ev /A> "));
	mappingProbabilityString.append(QString("Ev /A> "));
	for(int x = 0, size = maxActual; x < size; x++){
		AMGitHubIssue::ActualComplexityValue oneActual = AMGitHubIssue::ActualComplexityValue(x);
		double actualInt = AMGitHubIssue::integerFromActualComplexity(oneActual);
		indexProbabilityString.append(QString("%1").arg(actualInt, -7, 'f', 0, ' '));
		mappingProbabilityString.append(QString("%1").arg(actualInt, -7, 'f', 0, ' '));
	}
	indexProbabilityString.append("\n");
	mappingProbabilityString.append("\n");

	double runningTotal = 0;
	int rowCounter = 0;
	AMGitHubIssue::EstimatedComplexityValue oneEstimated = AMGitHubIssue::EstimatedComplexityValue(rowCounter);
	double estimatedInt = AMGitHubIssue::integerFromEstimatedComplexity(oneEstimated);
	indexProbabilityString.append(QString("%1").arg(estimatedInt, -7, 'f', 0, ' '));
	mappingProbabilityString.append(QString("%1").arg(estimatedInt, -7, 'f', 0, ' '));
	for(int x = 0, size = complexityMappingMatrix_.count(); x < size; x++){
		AMGitHubComplexityMapping *tempMapping = new AMGitHubComplexityMapping(x);
		indexProbabilityString.append(QString("%1").arg(probabilityOfMapping(tempMapping), -7, 'f', 3, ' '));
		mappingProbabilityString.append(QString("%1").arg(probabilityOfMappingInRow(tempMapping), -7, 'f', 3, ' '));
		runningTotal += probabilityOfMapping(tempMapping);
		if(x%maxEstimate == maxEstimate-1){
			indexProbabilityString.append("\n");
			mappingProbabilityString.append("\n");
			rowCounter++;
			AMGitHubIssue::EstimatedComplexityValue oneEstimated = AMGitHubIssue::EstimatedComplexityValue(rowCounter);
			if(oneEstimated != AMGitHubIssue::EstimatedComplexityInvalid){
				double estimatedInt = AMGitHubIssue::integerFromEstimatedComplexity(oneEstimated);
				indexProbabilityString.append(QString("%1").arg(estimatedInt, -7, 'f', 0, ' '));
				mappingProbabilityString.append(QString("%1").arg(estimatedInt, -7, 'f', 0, ' '));
			}
		}
	}
	indexProbabilityString.append(QString("Cumulative: %1").arg(runningTotal, 0, 'f', 3));
//	return indexProbabilityString;

	QString retVal = QString("%1\n\n%2").arg(indexProbabilityString).arg(mappingProbabilityString);
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

	int maxEstimate = int(AMGitHubIssue::EstimatedComplexityInvalid);
	int maxActual = int(AMGitHubIssue::ActualComplexityInvalid);
	complexityMappingMatrix_ = QVector<QList<AMGitHubIssue*> >(maxEstimate*maxActual);
	mappedIssues_.clear();
}
