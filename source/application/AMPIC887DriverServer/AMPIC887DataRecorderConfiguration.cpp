#include "AMPIC887DataRecorderConfiguration.h"
#include "AMGCS2Support.h"
AMPIC887DataRecorderConfiguration::AMPIC887DataRecorderConfiguration(int recordTableId,
																	 AMGCS2::DataRecordSource recordSource,
																	 AMGCS2::DataRecordOption recordOption)
{
	recordTableId_ = recordTableId;
	recordSource_ = recordSource;
	recordOption_ = recordOption;
}

bool AMPIC887DataRecorderConfiguration::isValid() const
{
	if(recordTableId_ < 1 || recordTableId_ > 16) {
		return false;
	}

	if(recordSource_ == AMGCS2::UnknownDataRecordSource) {
		return false;
	}

	if(recordOption_ == AMGCS2::UnknownRecordOption) {
		return false;
	}

	return true;
}

int AMPIC887DataRecorderConfiguration::recordTableId() const
{
	return recordTableId_;
}

AMGCS2::DataRecordSource AMPIC887DataRecorderConfiguration::recordSource() const
{
	return recordSource_;
}

AMGCS2::DataRecordOption AMPIC887DataRecorderConfiguration::recordOption() const
{
	return recordOption_;
}

QString AMPIC887DataRecorderConfiguration::toString() const
{
	return QString("%1\t\t%2\t\t%3")
			.arg(recordTableId_)
			.arg(AMGCS2Support::dataRecordSourceToString(recordSource_))
			.arg(AMGCS2Support::dataRecordOptionToString(recordOption_));
}
