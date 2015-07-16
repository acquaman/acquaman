#include "AMPIC887DataRecorderConfiguration.h"
#include "AMGCS2Support.h"
AMPIC887DataRecorderConfiguration::AMPIC887DataRecorderConfiguration(AMGCS2::DataRecordSource recordSource,
																	 AMGCS2::DataRecordOption recordOption)
{
	recordSource_ = recordSource;
	recordOption_ = recordOption;
}

bool AMPIC887DataRecorderConfiguration::isValid() const
{
	if(recordSource_ == AMGCS2::UnknownDataRecordSource) {
		return false;
	}

	if(recordOption_ == AMGCS2::UnknownRecordOption) {
		return false;
	}

	return true;
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
	return QString("%1\t\t\t%2 (%3)")
			.arg(AMGCS2Support::dataRecordSourceToString(recordSource_))
			.arg(AMGCS2Support::dataRecordOptionToString(recordOption_))
			.arg(AMGCS2Support::dataRecordOptionToInt(recordOption_));
}
