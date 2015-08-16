#include "AMGitHubComplexityManagerView.h"

#include <QStandardItemModel>
#include <QTableView>
#include <QBoxLayout>

AMGitHubComplexityManagerView::AMGitHubComplexityManagerView(AMGitHubComplexityManager *complexityManager, QWidget *parent) :
	QWidget(parent)
{
	complexityManager_ = complexityManager;

	probabilityOfMappingModel_ = new QStandardItemModel();
	probabilityOfMappingInRowModel_ = new QStandardItemModel();

	QList<QStandardItem*> oneProbabilityOfMappingModelRow;
	QList<QStandardItem*> oneProbabilityOfMappingInRowModelRow;
	AMGitHubComplexityMapping *oneMapping;
	for(int x = 0, size = AMGitHubComplexityMapping::maximumIndex(); x < size; x++){
		oneMapping = new AMGitHubComplexityMapping(x);

		QStandardItem *oneProbabilityOfMappingItem = new QStandardItem(QString("%1").arg(complexityManager_->probabilityOfMapping(oneMapping)));
		QStandardItem *oneProbabilityOfMappingInRowItem = new QStandardItem(QString("%1").arg(complexityManager_->probabilityOfMappingInRow(oneMapping)));

		oneProbabilityOfMappingModelRow.append(oneProbabilityOfMappingItem);
		oneProbabilityOfMappingInRowModelRow.append(oneProbabilityOfMappingInRowItem);

		int maxEstimate = AMGitHubIssue::EstimatedComplexityInvalid;
		if(x%maxEstimate == maxEstimate-1){
			probabilityOfMappingModel_->appendRow(oneProbabilityOfMappingModelRow);
			probabilityOfMappingInRowModel_->appendRow(oneProbabilityOfMappingInRowModelRow);

			oneProbabilityOfMappingModelRow.clear();
			oneProbabilityOfMappingInRowModelRow.clear();
		}
	}

	for(int x = 0, size = AMGitHubIssue::ActualComplexityInvalid; x < size; x++){
		AMGitHubIssue::ActualComplexityValue oneActual = AMGitHubIssue::ActualComplexityValue(x);
		probabilityOfMappingModel_->setHeaderData(x, Qt::Horizontal, QString("Actual %1").arg(AMGitHubIssue::integerFromActualComplexity(oneActual)));
		probabilityOfMappingInRowModel_->setHeaderData(x, Qt::Horizontal, QString("Actual %1").arg(AMGitHubIssue::integerFromActualComplexity(oneActual)));
	}

	for(int x = 0, size = AMGitHubIssue::EstimatedComplexityInvalid; x < size; x++){
		AMGitHubIssue::EstimatedComplexityValue oneEstimate = AMGitHubIssue::EstimatedComplexityValue(x);
		probabilityOfMappingModel_->setHeaderData(x, Qt::Vertical, QString("Estimated %1").arg(AMGitHubIssue::integerFromEstimatedComplexity(oneEstimate)));
		probabilityOfMappingInRowModel_->setHeaderData(x, Qt::Vertical, QString("Estimated %1").arg(AMGitHubIssue::integerFromEstimatedComplexity(oneEstimate)));
	}

	probabilityOfMappingTableView_ = new QTableView();
	probabilityOfMappingTableView_->setModel(probabilityOfMappingModel_);

	probabilityOfMappingInRowTableView_ = new QTableView();
	probabilityOfMappingInRowTableView_->setModel(probabilityOfMappingInRowModel_);

	QVBoxLayout *mainVL = new QVBoxLayout();
	mainVL->addWidget(probabilityOfMappingTableView_);
	mainVL->addWidget(probabilityOfMappingInRowTableView_);

	setLayout(mainVL);
}
