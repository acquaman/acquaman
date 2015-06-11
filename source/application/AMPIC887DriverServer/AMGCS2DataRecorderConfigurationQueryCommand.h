#ifndef AMGCS2DATARECORDERCONFIGURATIONQUERYCOMMAND_H
#define AMGCS2DATARECORDERCONFIGURATIONQUERYCOMMAND_H

#include <QList>
#include <QStringList>

#include "AMGCS2Command.h"
#include "AMPIC887DataRecorderConfiguration.h"
class AMGCS2DataRecorderConfigurationQueryCommand : public AMGCS2Command
{
public:
    AMGCS2DataRecorderConfigurationQueryCommand();

	virtual ~AMGCS2DataRecorderConfigurationQueryCommand() {}

	virtual QString outputString() const;

	QList<AMPIC887DataRecorderConfiguration*> dataRecorderConfigurations() const;
protected:

	bool runImplementation();

	QList<AMPIC887DataRecorderConfiguration*> dataRecorderConfigurations_;
};

#endif // AMGCS2DATARECORDERCONFIGURATIONQUERYCOMMAND_H
