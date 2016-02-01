#include "AMSortFilterScansWidget.h"
#include "dataman/AMLightweightScanInfoFilterProxyModel.h"

AMSortFilterScansWidget::AMSortFilterScansWidget(AMLightweightScanInfoFilterProxyModel *model, QWidget *parent) :
	AMSortFilterWidget(model, parent)
{
	if(!filterBuilder_->formLayout())
		return;

	model_ = model;
	runComboBox_ = new QComboBox();

	connect(runComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onRunSelected(int)));
	connect(model_, SIGNAL(runMapUpdated()), this, SLOT(refreshRunComboBox()));
	runComboBox_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	filterBuilder_->formLayout()->addRow("Run: ", runComboBox_);
	refreshRunComboBox();
}

void AMSortFilterScansWidget::setRunId(int runId)
{
	if(runId == -1)
		runComboBox_->setCurrentIndex(0);
	else
		runComboBox_->setCurrentIndex(runId);
}

void AMSortFilterScansWidget::refreshRunComboBox()
{
	runComboBox_->clear();

	if(model()) {
		const QHash<int, QString> runs = model_->runMap();

		runComboBox_->addItem("All Runs", -1);
		for(int iCurrentRun = 0, runCount = runs.count();
			iCurrentRun < runCount;
			++iCurrentRun) {

			runComboBox_->addItem(runs.value(iCurrentRun +1), iCurrentRun+1);
		}
	}
}

void AMSortFilterScansWidget::onRunSelected(int index)
{
	AMLightweightScanInfoFilterProxyModel * scanModel =
			qobject_cast<AMLightweightScanInfoFilterProxyModel*>(model());

	if(scanModel) {
		QVariant selectedRunIdVariant = runComboBox_->itemData(index);
		scanModel->setRunId(selectedRunIdVariant.toInt());
	}
}
