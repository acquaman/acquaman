#include "AMExporterOption.h"

AMExporterOption::AMExporterOption(QObject *parent) :
	AMDbObject(parent)
{
	fileName_ = "$name_$number_$dateTime.dat";
}
