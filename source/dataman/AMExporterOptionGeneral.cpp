#include "AMExporterOptionGeneral.h"

AMExporterOptionGeneral::AMExporterOptionGeneral(QObject *parent) :
	AMExporterOption(parent)
{

	headerText_ =	"Scan:\t\\name #\\number\n"
					"Date:\t\\dateTime\n"
					"Sample:\t\\sample\n"
					"Facility:\t\\facility\n\n";

	columnHeader_ =	"\\dataSource[description] (\\dataSource[units])";

	isColumnHeaderIncluded_ = true;

	columnHeaderDelimiter_ = "==========";

	sectionDelimiter_ = "\n\\dataSource[description] (\\dataSource[units])\n"
						"----------";
}

