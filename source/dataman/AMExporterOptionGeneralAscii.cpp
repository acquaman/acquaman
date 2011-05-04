#include "AMExporterOptionGeneralAscii.h"
#include <QPushButton>
#include "ui/AMExporterOptionGeneralAsciiView.h"

AMExporterOptionGeneralAscii::AMExporterOptionGeneralAscii(QObject *parent) :
	AMExporterOptionGeneral(parent)
{
	columnDelimiter_ = "\t";
	newlineDelimiter_ = "\r\n";

	fileName_ = "$name_$number_$dateTime[yyyyMMdd_hhmmss].txt";
	separateSectionFileName_ = "$name_$number_$dataSource_$dateTime[yyyyMMdd_hhmmss].txt";

	setModified(false);
}

QWidget * AMExporterOptionGeneralAscii::createEditorWidget()
{
	return new AMExporterOptionGeneralAsciiView(this, availableDataSourcesModel_);
}
