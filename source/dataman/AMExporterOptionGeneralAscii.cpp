#include "AMExporterOptionGeneralAscii.h"
#include <QPushButton>
#include "ui/AMExporterOptionGeneralAsciiView.h"

AMExporterOptionGeneralAscii::AMExporterOptionGeneralAscii(QObject *parent) :
	AMExporterOptionGeneral(parent)
{
	columnDelimiter_ = "\t";
	newlineDelimiter_ = "\r\n";

	fileName_ = "$name_$number_$dateTime.txt";
}

QWidget * AMExporterOptionGeneralAscii::createEditorWidget()
{
	return new AMExporterOptionGeneralAsciiView(this);
}
