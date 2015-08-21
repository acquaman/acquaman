#include "AMStripToolWidget.h"

AMStripToolWidget::AMStripToolWidget(AMStripTool *stripTool, QWidget *parent) :
	AMStripToolView(parent)
{
	// Initialize variables.

	stripTool_ = 0;

	// Create UI elements.

	plotWidget_ = new MPlotWidget();

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(plotWidget_);

	setLayout(layout);

	// Set context menu.

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect( this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)) );

	// Current settings.

	setStripTool(stripTool);
}

AMStripToolWidget::~AMStripToolWidget()
{

}

void AMStripToolWidget::setStripTool(AMStripTool *newTool)
{
	if (stripTool_ != newTool) {

		if (stripTool_) {
			plotWidget_->setPlot(0);
		}

		stripTool_ = newTool;

		if (stripTool_) {
			plotWidget_->setPlot(stripTool_->plot());
		}

		updateGeometry();

		emit stripToolChanged(stripTool_);
	}
}

void AMStripToolWidget::showAddPVDialog()
{
	if (stripTool_) {
		bool ok = false;
		QString pvName = QInputDialog::getText(this, "Add PV", "PV name:", QLineEdit::Normal, "", &ok);

		if (ok && !pvName.isEmpty())
			stripTool_->addItem(pvName);
	}
}

void AMStripToolWidget::onCustomContextMenuRequested(QPoint point)
{
	QMenu menu(this);

	// Add menu options.

	QAction *addPVAction = menu.addAction("Add PV");

	// Execute selected option.

	QAction *selection = menu.exec(mapToGlobal(point));

	if (selection) {
		if (selection->text() == addPVAction->text())
			showAddPVDialog();
	}
}
