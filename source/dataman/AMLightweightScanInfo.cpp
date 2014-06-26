#include "AMLightweightScanInfo.h"

AMLightweightScanInfo::AMLightweightScanInfo(
		int id,
		const QString &name,
		int number, const
		QDateTime &dateTime,
		const QString &scanType,
		const QString &runName,
		const QString &notes,
		const QString &sampleName)
{
	id_ = id;
	name_ = name;
	number_ = number;
	dateTime_ = dateTime;
	scanType_ = scanType;
	runName_ = runName;
	notes_ = notes;
	sampleName_ = sampleName;
}

int AMLightweightScanInfo::id() const
{
	return id_;
}

QString AMLightweightScanInfo::name() const
{
	return name_;
}

int AMLightweightScanInfo::number() const
{
	return number_;
}

QDateTime AMLightweightScanInfo::dateTime() const
{
	return dateTime_;
}

QString AMLightweightScanInfo::scanType() const
{
	return scanType_;
}

QString AMLightweightScanInfo::runName() const
{
	return runName_;
}

QString AMLightweightScanInfo::notes() const
{
	return notes_;
}

QString AMLightweightScanInfo::sampleName() const
{
	return sampleName_;
}
