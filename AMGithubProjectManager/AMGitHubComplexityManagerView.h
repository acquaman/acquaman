#ifndef AMGITHUBCOMPLEXITYMANAGERVIEW_H
#define AMGITHUBCOMPLEXITYMANAGERVIEW_H

#include <QWidget>
#include <QGroupBox>

#include "AMGitHubComplexityManager.h"

class QTableView;
class QStandardItemModel;

class AMGitHubComplexityValueView;

class AMGitHubComplexityManagerView : public QWidget
{
Q_OBJECT
public:
	AMGitHubComplexityManagerView(AMGitHubComplexityManager *complexityManager, QWidget *parent = 0);

protected slots:
	void adjustTableSizes();

protected:
	AMGitHubComplexityManager *complexityManager_;

	QStandardItemModel *probabilityOfMappingModel_;
	QTableView *probabilityOfMappingTableView_;
	QStandardItemModel *probabilityOfMappingInRowModel_;
	QTableView *probabilityOfMappingInRowTableView_;

	QList<AMGitHubComplexityValueView*> complexityValueViews_;
};

class AMGitHubComplexityValueView : public QGroupBox
{
Q_OBJECT
public:
	AMGitHubComplexityValueView(AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue, AMGitHubComplexityManager *complexityManager, QWidget *parent = 0);

protected:
	AMGitHubIssue::EstimatedComplexityValue estimatedComplexityValue_;
	AMGitHubIssue::ActualComplexityValue equivalentActualComplexityValue_;
	AMGitHubComplexityManager *complexityManager_;
};

#endif // AMGITHUBCOMPLEXITYMANAGERVIEW_H
