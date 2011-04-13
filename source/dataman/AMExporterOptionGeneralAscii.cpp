#include "AMExporterOptionGeneralAscii.h"

AMExporterOptionGeneralAscii::AMExporterOptionGeneralAscii(QObject *parent) :
	AMExporterOptionGeneral(parent)
{
	columnDelimiter_ = "\t";
	newlineDelimiter_ = "\r\n";

	fileName_ = "\\name_\\number[3]_\\dateTime.txt";
}
