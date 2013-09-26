#include "AMXRFDetailedDetectorView.h"

#include <QHBoxLayout>

AMXRFDetailedDetectorView::AMXRFDetailedDetectorView(AMXRFDetector *detector, QWidget *parent)
	: AMXRFBaseDetectorView(detector, parent)
{
	periodicTable_ = new AMSelectablePeriodicTable(this);
	periodicTable_->buildPeriodicTable();
}

void AMXRFDetailedDetectorView::buildDetectorView()
{
	AMXRFBaseDetectorView::buildDetectorView();

	periodicTableView_ = new AMSelectablePeriodicTableView(periodicTable_);
	periodicTableView_->buildPeriodicTableView();
	elementView_ = new AMSelectableElementView(qobject_cast<AMSelectableElement *>(periodicTable_->elementBySymbol("Fe")));

	QHBoxLayout *periodicTableAndElementViewLayout = new QHBoxLayout;
	periodicTableAndElementViewLayout->addWidget(periodicTableView_);
	periodicTableAndElementViewLayout->addWidget(elementView_);

	bottomLayout_->addLayout(periodicTableAndElementViewLayout);
}
