#ifndef AMGCS2GETDATARECORDERCONFIGURATIONCOMMAND_H
#define AMGCS2GETDATARECORDERCONFIGURATIONCOMMAND_H

#include <QList>
#include <QStringList>

#include "AMGCS2Command.h"
#include "AMPIC887DataRecorderConfiguration.h"
class AMGCS2GetDataRecorderConfigurationCommand : public AMGCS2Command
{
public:
	AMGCS2GetDataRecorderConfigurationCommand();

	virtual ~AMGCS2GetDataRecorderConfigurationCommand() {}

	virtual QString outputString() const;

	QList<AMPIC887DataRecorderConfiguration*> dataRecorderConfigurations() const;
protected:

	bool runImplementation();

	QList<AMPIC887DataRecorderConfiguration*> dataRecorderConfigurations_;
};

#endif // AMGCS2GETDATARECORDERCONFIGURATIONCOMMAND_H
