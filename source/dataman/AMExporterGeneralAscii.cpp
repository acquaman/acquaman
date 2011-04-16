#include "AMExporterGeneralAscii.h"
#include "AMExporterOptionGeneralAscii.h"

AMExporterGeneralAscii::AMExporterGeneralAscii(QObject *parent) :
	AMExporter(parent)
{
}

AMExporterOption * AMExporterGeneralAscii::createDefaultOption() const {
	return new AMExporterOptionGeneralAscii();
}
