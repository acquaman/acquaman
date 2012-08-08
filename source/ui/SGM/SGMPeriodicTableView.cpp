#include "SGMPeriodicTableView.h"

#include <QBoxLayout>
#include <QMenu>

#include "ui/dataman/AMDbObjectGeneralView.h"
#include "ui/dataman/AMDbObjectGeneralViewSupport.h"

SGMPeriodicTableView::SGMPeriodicTableView(SGMPeriodicTable *sgmPeriodicTable, QWidget *parent) :
	QWidget(parent)
{
	sgmPeriodicTable_ = sgmPeriodicTable;
	periodicTableView_ = new AMPeriodicTableView(this);

	const AMElement *tempElement;
	for(int x = 0; x < AMPeriodicTable::table()->elements().count(); x++){
		tempElement = AMPeriodicTable::table()->elements().at(x);
		periodicTableView_->button(tempElement)->setEnabled(false);
		if( (tempElement->KEdge().second.toDouble() > 200 && tempElement->KEdge().second.toDouble() < 2000) || (tempElement->L2Edge().second.toDouble() > 200 && tempElement->L2Edge().second.toDouble() < 2000) || (tempElement->M3Edge().second.toDouble() > 200 && tempElement->M3Edge().second.toDouble() < 2000) )
			periodicTableView_->button(tempElement)->setEnabled(true);
	}

	connect(periodicTableView_, SIGNAL(clicked(int)), this, SLOT(onClicked(int)));

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(periodicTableView_);
	setLayout(vl);
}

void SGMPeriodicTableView::onClicked(int atomicNumber){
	qDebug() << "Clicked on atomic number " << atomicNumber;

	QString elementName = AMPeriodicTable::table()->elementByAtomicNumber(atomicNumber)->name();
	QList<SGMFastScanParameters*> allFastScans = SGMPeriodicTable::sgmTable()->fastScanPresets();

	if(availableScansMenu_)
		delete availableScansMenu_;
	availableScansMenu_ = new QMenu();
	QAction *tempAction;
	bool foundOne = false;
	for(int x = 0; x < allFastScans.count(); x++){
		if(allFastScans.at(x)->element() == elementName){
			qDebug() << "Found that element " << elementName << " at index " << x;
			foundOne = true;

			tempAction = availableScansMenu_->addAction(allFastScans.at(x)->name());
			tempAction->setData(x);
			connect(tempAction, SIGNAL(hovered()), this, SLOT(onHoveredAvailableScansMenu()));
			connect(tempAction, SIGNAL(triggered()), this, SLOT(onTriggerAvailableScansMenu()));
		}
	}
	if(!foundOne){
		tempAction = availableScansMenu_->addAction("None Available");
		tempAction->setEnabled(false);
	}

	availableScansMenu_->popup(QCursor::pos());
	availableScansMenu_->show();
}

void SGMPeriodicTableView::onHoveredAvailableScansMenu(){
	QAction *tempAction;
	tempAction = availableScansMenu_->activeAction();
	activeMenuData_ = tempAction->data().toInt();
}

void SGMPeriodicTableView::onTriggerAvailableScansMenu(){
	SGMFastScanParameters *fastScanToView = SGMPeriodicTable::sgmTable()->fastScanPresets().at(activeMenuData_);

	AMDbObjectGeneralView *generalViewForFastScanParameters;
	generalViewForFastScanParameters = AMDbObjectGeneralViewSupport::closestMatchingGeneralView(fastScanToView);
	generalViewForFastScanParameters->show();
}
