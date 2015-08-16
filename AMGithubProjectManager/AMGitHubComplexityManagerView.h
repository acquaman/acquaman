#ifndef AMGITHUBCOMPLEXITYMANAGERVIEW_H
#define AMGITHUBCOMPLEXITYMANAGERVIEW_H

#include <QWidget>

#include "AMGitHubComplexityManager.h"

class QTableView;
class QStandardItemModel;

class AMGitHubComplexityManagerView : public QWidget
{
Q_OBJECT
public:
	AMGitHubComplexityManagerView(AMGitHubComplexityManager *complexityManager, QWidget *parent = 0);

protected:
	AMGitHubComplexityManager *complexityManager_;

	QStandardItemModel *probabilityOfMappingModel_;
	QTableView *probabilityOfMappingTableView_;
	QStandardItemModel *probabilityOfMappingInRowModel_;
	QTableView *probabilityOfMappingInRowTableView_;


};

#endif // AMGITHUBCOMPLEXITYMANAGERVIEW_H
