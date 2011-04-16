#include "AMExporterOptionGeneralAsciiView.h"
#include "ui_AMExporterOptionGeneralAsciiView.h"

#include "dataman/AMExporterOptionGeneralAscii.h"

AMExporterOptionGeneralAsciiView::AMExporterOptionGeneralAsciiView(AMExporterOptionGeneralAscii* option, QWidget *parent) :
	QTabWidget(parent)
{
	setupUi(this);
}

AMExporterOptionGeneralAsciiView::~AMExporterOptionGeneralAsciiView()
{
}
