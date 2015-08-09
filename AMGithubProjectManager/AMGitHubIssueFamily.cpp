#include "AMGitHubIssueFamily.h"

#include <QRegExp>

AMGitHubIssueFamily::AMGitHubIssueFamily(AMGitHubIssue *originatingIssue, AMGitHubIssue *pullRequestIssue, QObject *parent) :
	QObject(parent)
{
	originatingIssue_ = originatingIssue;
	pullRequestIssue_ = pullRequestIssue;
}

AMGitHubIssueFamily::~AMGitHubIssueFamily()
{
}

int AMGitHubIssueFamily::originatingIssueNumber() const
{
	if(originatingIssue_)
		return originatingIssue_->issueNumber();
	return -1;
}

int AMGitHubIssueFamily::pullRequestIssueNumber() const
{
	if(pullRequestIssue_)
		return pullRequestIssue_->issueNumber();
	return -1;
}

AMGitHubIssue::ActualComplexityValue AMGitHubIssueFamily::estimatedComplexity() const
{
	if(originatingIssue_)
		return originatingIssue_->actualComplexityValue();
	return AMGitHubIssue::ActualComplexityInvalid;
}

AMGitHubIssue::ActualComplexityValue AMGitHubIssueFamily::actualComplexity() const
{
	if(pullRequestIssue_)
		return pullRequestIssue_->actualComplexityValue();
	return AMGitHubIssue::ActualComplexityInvalid;
}

QString AMGitHubIssueFamily::timeEstimate() const
{
	if(originatingIssue_)
		return originatingIssue_->timeEstimateString();
	return "Invalid Time Estimate";
}

QString AMGitHubIssueFamily::multiLineDebugInfo() const
{
	QString retVal;

	if(originatingIssue_)
		retVal.append(QString("Original:\t\t%1\n").arg(originatingIssueNumber()));
	else
		retVal.append(QString("Original:\t\tMISSING\n"));

	if(pullRequestIssue_)
		retVal.append(QString("Pull Request:\t%1\n").arg(pullRequestIssueNumber()));
	else
		retVal.append(QString("Pull Request:\tMISSING\n"));

	if(originatingIssue_ && estimatedComplexity() != AMGitHubIssue::ActualComplexityInvalid)
		retVal.append(QString("Estimated Complexity:\t%1\n").arg(AMGitHubIssue::stringFromActualComplexity(estimatedComplexity())));
	else
		retVal.append(QString("Estimated Complexity:\tMISSING\n"));

	if(pullRequestIssue_ && actualComplexity() != AMGitHubIssue::ActualComplexityInvalid)
		retVal.append(QString("Actual Complexity:\t%1\n").arg(AMGitHubIssue::stringFromActualComplexity(actualComplexity())));
	else
		retVal.append(QString("Actual Complexity:\tMISSING\n"));

	if(originatingIssue_ && timeEstimate() != "Invalid Time Estimated" && !timeEstimate().isEmpty())
		retVal.append(QString("Time Estimate:\t%1\n").arg(timeEstimate()));
	else
		retVal.append(QString("Time Estimate:\tMISSING\n"));

	return retVal;

}

bool AMGitHubIssueFamily::completeInfo() const
{
	if(originatingIssue_ && originatingIssue()->projectTrackingDisabled())
		return false;

	if(!originatingIssue_ || !pullRequestIssue_)
		return false;

	if(originatingIssue_ && estimatedComplexity() == AMGitHubIssue::ActualComplexityInvalid)
		return false;

	if(pullRequestIssue_ && actualComplexity() == AMGitHubIssue::ActualComplexityInvalid)
		return false;

	if(originatingIssue_ && (timeEstimate() == "Invalid Time Estimated" || timeEstimate().isEmpty()) )
		return false;

	return true;
}

bool AMGitHubIssueFamily::totallyIncomplete() const{
	if(originatingIssue_ && originatingIssue()->projectTrackingDisabled())
		return false;

	if(!pullRequestIssue_ && estimatedComplexity() == AMGitHubIssue::ActualComplexityInvalid && (timeEstimate() == "Invalid Time Estimated" || timeEstimate().isEmpty()))
		return true;

	return false;
}

bool AMGitHubIssueFamily::onlyMissingActualComplexity() const{
	if(originatingIssue_ && originatingIssue()->projectTrackingDisabled())
		return false;

	if(originatingIssue_ && pullRequestIssue_ && (estimatedComplexity() != AMGitHubIssue::ActualComplexityInvalid) && (timeEstimate() != "Invalid Time Estimated") && !timeEstimate().isEmpty() && actualComplexity() == AMGitHubIssue::ActualComplexityInvalid)
		return true;
	return false;
}

bool AMGitHubIssueFamily::onlyMissingTimeEstimate() const{
	if(originatingIssue_ && originatingIssue()->projectTrackingDisabled())
		return false;

	if(originatingIssue_ && pullRequestIssue_ && (estimatedComplexity() != AMGitHubIssue::ActualComplexityInvalid) && (timeEstimate() == "Invalid Time Estimated" || timeEstimate().isEmpty()) && (actualComplexity() != AMGitHubIssue::ActualComplexityInvalid) )
		return true;
	return false;
}

AMGitHubIssueFamily::ComplexityMapping AMGitHubIssueFamily::complexityMapping() const{
	AMGitHubIssue::ActualComplexityValue localEstimatedComplexity = estimatedComplexity();
	AMGitHubIssue::ActualComplexityValue localActualComplexity = actualComplexity();

	switch(localEstimatedComplexity){
	case AMGitHubIssue::ActualComplexity1:{
		switch(localActualComplexity){
		case AMGitHubIssue::ActualComplexity1:
			return AMGitHubIssueFamily::Complexity1ToComplexity1;
		case AMGitHubIssue::ActualComplexity2:
			return AMGitHubIssueFamily::Complexity1ToComplexity2;
		case AMGitHubIssue::ActualComplexity3:
			return AMGitHubIssueFamily::Complexity1ToComplexity3;
		case AMGitHubIssue::ActualComplexity5:
			return AMGitHubIssueFamily::Complexity1ToComplexity5;
		case AMGitHubIssue::ActualComplexity8:
			return AMGitHubIssueFamily::Complexity1ToComplexity8;
//		case AMGitHubIssue::ComplexityK:
//			return AMGitHubIssueFamily::InvalidComplexityMapping;
		default:
			return AMGitHubIssueFamily::InvalidComplexityMapping;
		}
		break;}
	case AMGitHubIssue::ActualComplexity2:{
		switch(localActualComplexity){
		case AMGitHubIssue::ActualComplexity1:
			return AMGitHubIssueFamily::Complexity2ToComplexity1;
		case AMGitHubIssue::ActualComplexity2:
			return AMGitHubIssueFamily::Complexity2ToComplexity2;
		case AMGitHubIssue::ActualComplexity3:
			return AMGitHubIssueFamily::Complexity2ToComplexity3;
		case AMGitHubIssue::ActualComplexity5:
			return AMGitHubIssueFamily::Complexity2ToComplexity5;
		case AMGitHubIssue::ActualComplexity8:
			return AMGitHubIssueFamily::Complexity2ToComplexity8;
//		case AMGitHubIssue::ComplexityK:
//			return AMGitHubIssueFamily::InvalidComplexityMapping;
		default:
			return AMGitHubIssueFamily::InvalidComplexityMapping;
		}
		break;}
	case AMGitHubIssue::ActualComplexity3:{
		switch(localActualComplexity){
		case AMGitHubIssue::ActualComplexity1:
			return AMGitHubIssueFamily::Complexity3ToComplexity1;
		case AMGitHubIssue::ActualComplexity2:
			return AMGitHubIssueFamily::Complexity3ToComplexity2;
		case AMGitHubIssue::ActualComplexity3:
			return AMGitHubIssueFamily::Complexity3ToComplexity3;
		case AMGitHubIssue::ActualComplexity5:
			return AMGitHubIssueFamily::Complexity3ToComplexity5;
		case AMGitHubIssue::ActualComplexity8:
			return AMGitHubIssueFamily::Complexity3ToComplexity8;
//		case AMGitHubIssue::ComplexityK:
//			return AMGitHubIssueFamily::InvalidComplexityMapping;
		default:
			return AMGitHubIssueFamily::InvalidComplexityMapping;
		}
		break;}
	case AMGitHubIssue::ActualComplexity5:{
		switch(localActualComplexity){
		case AMGitHubIssue::ActualComplexity1:
			return AMGitHubIssueFamily::Complexity5ToComplexity1;
		case AMGitHubIssue::ActualComplexity2:
			return AMGitHubIssueFamily::Complexity5ToComplexity2;
		case AMGitHubIssue::ActualComplexity3:
			return AMGitHubIssueFamily::Complexity5ToComplexity3;
		case AMGitHubIssue::ActualComplexity5:
			return AMGitHubIssueFamily::Complexity5ToComplexity5;
		case AMGitHubIssue::ActualComplexity8:
			return AMGitHubIssueFamily::Complexity5ToComplexity8;
//		case AMGitHubIssue::ComplexityK:
//			return AMGitHubIssueFamily::InvalidComplexityMapping;
		default:
			return AMGitHubIssueFamily::InvalidComplexityMapping;
		}
		break;}
	case AMGitHubIssue::ActualComplexity8:{
		switch(localActualComplexity){
		case AMGitHubIssue::ActualComplexity1:
			return AMGitHubIssueFamily::Complexity8ToComplexity1;
		case AMGitHubIssue::ActualComplexity2:
			return AMGitHubIssueFamily::Complexity8ToComplexity2;
		case AMGitHubIssue::ActualComplexity3:
			return AMGitHubIssueFamily::Complexity8ToComplexity3;
		case AMGitHubIssue::ActualComplexity5:
			return AMGitHubIssueFamily::Complexity8ToComplexity5;
		case AMGitHubIssue::ActualComplexity8:
			return AMGitHubIssueFamily::Complexity8ToComplexity8;
//		case AMGitHubIssue::ComplexityK:
//			return AMGitHubIssueFamily::InvalidComplexityMapping;
		default:
			return AMGitHubIssueFamily::InvalidComplexityMapping;
		}
		break;}
//	case AMGitHubIssue::ComplexityK:{
//		switch(localActualComplexity){
//		case AMGitHubIssue::ActualComplexity1:
//			return AMGitHubIssueFamily::ComplexityKToComplexity1;
//		case AMGitHubIssue::ActualComplexity2:
//			return AMGitHubIssueFamily::ComplexityKToComplexity2;
//		case AMGitHubIssue::ActualComplexity3:
//			return AMGitHubIssueFamily::ComplexityKToComplexity3;
//		case AMGitHubIssue::ActualComplexity5:
//			return AMGitHubIssueFamily::ComplexityKToComplexity5;
//		case AMGitHubIssue::ActualComplexity8:
//			return AMGitHubIssueFamily::ComplexityKToComplexity8;
////		case AMGitHubIssue::ComplexityK:
////			return AMGitHubIssueFamily::InvalidComplexityMapping;
//		default:
//			return AMGitHubIssueFamily::InvalidComplexityMapping;
//		}
//		break;}
	case AMGitHubIssue::ActualComplexityInvalid:
		return AMGitHubIssueFamily::InvalidComplexityMapping;
	default:
		return AMGitHubIssueFamily::InvalidComplexityMapping;
	}
}

bool AMGitHubIssueFamily::symmetricComplexityMapping() const{
	if(complexityMapping() == AMGitHubIssueFamily::Complexity1ToComplexity1 || complexityMapping() == AMGitHubIssueFamily::Complexity2ToComplexity2 || complexityMapping() == AMGitHubIssueFamily::Complexity3ToComplexity3 || complexityMapping() == AMGitHubIssueFamily::Complexity5ToComplexity5 || complexityMapping() == AMGitHubIssueFamily::Complexity8ToComplexity8)
		return true;
	return false;
}

double AMGitHubIssueFamily::normalizedTimeEstiamte() const{
	QString timeEstimateFirstHalf = timeEstimate().section(' ', 0, 0);
	QString timeEstimateSecondHalf = timeEstimate().section(' ', 1, 1);

	bool convertsToDouble = false;
	double timeValue = timeEstimateFirstHalf.toDouble(&convertsToDouble);
	if(!convertsToDouble)
		return -1;

	double multiplier;
	QRegExp secondRX("[Ss]econd");
	QRegExp minuteRX("[Mm]inute");
	QRegExp hourRX("[Hh]our");
	QRegExp dayRX("[Dd]ay");
	QRegExp weekRX("[Ww]eek");
	QRegExp monthRX("[Mm]onth");
	if(timeEstimateSecondHalf.contains(secondRX))
		multiplier = 1.0/60.0/60.0;
	else if(timeEstimateSecondHalf.contains(minuteRX))
		multiplier = 1.0/60.0;
	else if(timeEstimateSecondHalf.contains(hourRX))
		multiplier = 1.0;
	else if(timeEstimateSecondHalf.contains(dayRX))
		multiplier = 1.0*8.0;
	else if(timeEstimateSecondHalf.contains(weekRX))
		multiplier = 1.0*8.0*5.0;
	else if(timeEstimateSecondHalf.contains(monthRX))
		multiplier = 1.0*8.0*5.0*4.0;
	else
		return -1;

	double retVal = timeValue*multiplier;
	if(retVal < 1.0)
		retVal = 1.0;
	return retVal;
}
