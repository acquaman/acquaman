#include "AMStripToolEditor.h"

AMStripToolEditor::AMStripToolEditor(QWidget *parent) :
	AMStripToolView(parent)
{
	// Initialize class variables.

	stripTool_ = new AMStripTool();

	scan_ = new AMScan();

	AMScanSetModel *scanSetModel = new AMScanSetModel(this);
	scanSetModel->addScan(scan_);

	// Create UI elements.

	stripToolView_ = new AMScanView(scanSetModel);

	sourcesEditor_ = new AMDataSourcesEditor(scanSetModel);

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(stripToolView_);
	layout->addWidget(sourcesEditor_);

	setLayout(layout);
}

AMStripToolEditor::~AMStripToolEditor()
{

}
