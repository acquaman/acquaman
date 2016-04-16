#include "AMGitHubComplexityManagerView.h"

#include <QStandardItemModel>
#include <QTableView>
#include <QBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QScrollArea>
#include <QGridLayout>
#include <QTimer>

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

		QStandardItem *oneProbabilityOfMappingItem = new QStandardItem(QString("%1%").arg(complexityManager_->probabilityOfMapping(oneMapping)*100, 0, 'f', 2));
		QStandardItem *oneProbabilityOfMappingInRowItem = new QStandardItem(QString("%1%").arg(complexityManager_->probabilityOfMappingInRow(oneMapping)*100, 0, 'f', 2));

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

	QGroupBox *overallProbabilityGroupBox = new QGroupBox("Overall Probability");
	probabilityOfMappingTableView_ = new QTableView();
	probabilityOfMappingTableView_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	probabilityOfMappingTableView_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	probabilityOfMappingTableView_->setModel(probabilityOfMappingModel_);
	QVBoxLayout *overallProbabilityVL = new QVBoxLayout;
	overallProbabilityVL->addWidget(probabilityOfMappingTableView_);
	overallProbabilityGroupBox->setLayout(overallProbabilityVL);

	QGroupBox *rowProbabilityGroupBox = new QGroupBox("Row Probability");
	probabilityOfMappingInRowTableView_ = new QTableView();
	probabilityOfMappingInRowTableView_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	probabilityOfMappingInRowTableView_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	probabilityOfMappingInRowTableView_->setModel(probabilityOfMappingInRowModel_);
	QVBoxLayout *rowProbabilityVL = new QVBoxLayout;
	rowProbabilityVL->addWidget(probabilityOfMappingInRowTableView_);
	rowProbabilityGroupBox->setLayout(rowProbabilityVL);

	QVBoxLayout *mainVL = new QVBoxLayout();
	mainVL->addWidget(new QLabel(QString("Total Analyzed Issues: %1").arg(complexityManager_->analyzedIssues())));
	mainVL->addWidget(overallProbabilityGroupBox);
	mainVL->addStretch();
	mainVL->addWidget(rowProbabilityGroupBox);
	mainVL->addStretch();


	QGridLayout *complexityValueViewsGridLayout = new QGridLayout;
	AMGitHubComplexityValueView *oneComplexityValueView;
	for(int x = 0, size = int(AMGitHubIssue::EstimatedComplexityInvalid); x < size; x++){
		AMGitHubIssue::EstimatedComplexityValue oneEstimatedComplexityValue = AMGitHubIssue::EstimatedComplexityValue(x);
		oneComplexityValueView = new AMGitHubComplexityValueView(oneEstimatedComplexityValue, complexityManager_);
		complexityValueViews_.append(oneComplexityValueView);
	}
	int columnCounter = 0;
	int rowCounter = 0;
	for(int x = 0, size = complexityValueViews_.count(); x < size; x++){
		complexityValueViewsGridLayout->addWidget(complexityValueViews_.at(x), rowCounter, columnCounter);
		columnCounter++;
		if(columnCounter%3 == 0){
			columnCounter = 0;
			rowCounter++;
		}
	}
	mainVL->addLayout(complexityValueViewsGridLayout);

	setLayout(mainVL);

	QTimer::singleShot(0, this, SLOT(adjustTableSizes()));
}

void AMGitHubComplexityManagerView::adjustTableSizes()
{
	probabilityOfMappingTableView_->setMinimumSize(950, 250);
	probabilityOfMappingInRowTableView_->setMinimumSize(950, 250);
}

AMGitHubComplexityValueView::AMGitHubComplexityValueView(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue, AMGitHubComplexityManager *complexityManager, QWidget *parent) :
	QGroupBox(parent)
{
	estimatedComplexityValue_ = estimatedComplexityValue;
	equivalentActualComplexityValue_ = AMGitHubIssue::correspondingActualComplexity(estimatedComplexityValue_);
	complexityManager_ = complexityManager;

	setTitle(QString("Complexity %1").arg(AMGitHubIssue::integerFromEstimatedComplexity(estimatedComplexityValue_)));

	QFormLayout *mainFL = new QFormLayout();

	QString averageTimeForEstimatedTitle = QString("Average Time for Estimated %1").arg(AMGitHubIssue::integerFromEstimatedComplexity(estimatedComplexityValue_));
	QString averageTimeForEstimatedValue = QString("%1 %2 %3").arg(complexityManager_->averageTimeForEstimatedComplexity(estimatedComplexityValue_), 0, 'f', 2).arg(QChar(0x00B1)).arg(complexityManager_->standardDeviationTimeForEstimatedComplexity(estimatedComplexityValue_), 0, 'f', 2);

	QString averageTimeForActualTitle = QString("Average Time for Actual %1").arg(AMGitHubIssue::integerFromActualComplexity(equivalentActualComplexityValue_));
	QString averageTimeForActualValue = QString("%1 %2 %3").arg(complexityManager_->averageTimeForActualComplexity(equivalentActualComplexityValue_), 0, 'f', 2).arg(QChar(0x00B1)).arg(complexityManager_->standardDeviationTimeForActualComplexity(equivalentActualComplexityValue_), 0, 'f', 2);

	QString probableTimeForEstimatedTitle = QString("Probable Time for Estimated %1").arg(AMGitHubIssue::integerFromEstimatedComplexity(estimatedComplexityValue_));
	QString probableTimeForEstimatedValue = QString("%1 %2 %3").arg(complexityManager_->probableTimeForEstimatedComplexity(estimatedComplexityValue_), 0, 'f', 2).arg(QChar(0x00B1)).arg(complexityManager_->probablestandardDeviationTimeForEstimatedComplexity(estimatedComplexityValue_), 0, 'f', 2);

	mainFL->addRow(averageTimeForEstimatedTitle, new QLabel(averageTimeForEstimatedValue));
	mainFL->addRow(averageTimeForActualTitle, new QLabel(averageTimeForActualValue));
	mainFL->addRow(probableTimeForEstimatedTitle, new QLabel(probableTimeForEstimatedValue));

	setLayout(mainFL);
}
