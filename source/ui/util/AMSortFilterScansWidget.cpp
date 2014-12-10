#include "AMSortFilterScansWidget.h"
#include "dataman/AMLightweightScanInfoFilterProxyModel.h"
#include <QSet>
AMSortFilterScansWidget::AMSortFilterScansWidget(AMLightweightScanInfoFilterProxyModel *model, QWidget *parent) :
	AMSortFilterWidget(model, parent)
{
	if(!filterBuilder_->formLayout())
		return;

	runComboBox_ = new QComboBox();

	connect(runComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onRunSelected(int)));

	runComboBox_->addItem("All Runs", -1);
	runComboBox_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	if(model)
	{
		const QHash<int, QString> runs = model->runMap();

		for(int iCurrentRun = 0, runCount = runs.count();
			iCurrentRun < runCount;
			++iCurrentRun)
		{
			runComboBox_->addItem(runs.value(iCurrentRun +1), iCurrentRun+1);
		}

	}


	filterBuilder_->formLayout()->addRow("Run: ", runComboBox_);
}

void AMSortFilterScansWidget::onRunSelected(int index)
{
	AMLightweightScanInfoFilterProxyModel * scanModel =
			qobject_cast<AMLightweightScanInfoFilterProxyModel*>(model());

	if(scanModel)
	{
		QVariant selectedRunIdVariant = runComboBox_->itemData(index);
		scanModel->setRunId(selectedRunIdVariant.toInt());
	}
}
