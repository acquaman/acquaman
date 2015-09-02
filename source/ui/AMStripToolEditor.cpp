#include "AMStripToolEditor.h"

AMStripToolEditor::AMStripToolEditor(AMStripTool *stripTool, QWidget *parent) :
	AMStripToolView(parent)
{
	// Initialize class variables.

	stripTool_ = 0;
	scan_ = 0;

	scanSetModel_ = new AMScanSetModel(this);

	// Create UI elements.

	stripToolView_ = new AMScanView(scanSetModel_);

	sourcesEditor_ = new AMDataSourcesEditor(scanSetModel_);

	itemEditor_ = new AMStripToolItemEditor(0);

	// Create and set layouts.

	QHBoxLayout *topLayout = new QHBoxLayout();
	topLayout->setMargin(0);
	topLayout->addWidget(stripToolView_);
	topLayout->addWidget(sourcesEditor_);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addLayout(topLayout);
	layout->addWidget(itemEditor_);

	setLayout(layout);

	// Current settings.

	setStripTool(stripTool);
}

AMStripToolEditor::~AMStripToolEditor()
{

}

void AMStripToolEditor::setStripTool(AMStripTool *newTool)
{
	if (stripTool_ != newTool) {

		if (stripTool_) {
			scanSetModel_->removeScan(scan_);
			scan_->deleteLater();
			scan_ = 0;
		}

		stripTool_ = newTool;

		if (stripTool_) {
			scan_ = createScan(stripTool_);
			scanSetModel_->addScan(scan_);
		}

		emit stripToolChanged(stripTool_);
	}
}

void AMStripToolEditor::setStripToolItem(AMStripToolItem *item)
{
	itemEditor_->setItem(item);
}

AMScan* AMStripToolEditor::createScan(AMStripTool *stripTool)
{
	AMScan *scan = 0;

	if (stripTool && stripTool->items()) {

		scan = new AMScan(this);

		QList<AMStripToolItem*> items = stripTool->items()->itemsList();

		foreach (AMStripToolItem *item, items) {
			if (item) {
				AMAnalysisBlock *dataSource = (AMAnalysisBlock*)(item->dataSource());
				scan->addAnalyzedDataSource(dataSource, true, false);
			}
		}
	}

	return scan;
}
