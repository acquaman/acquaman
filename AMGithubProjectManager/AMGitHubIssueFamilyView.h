#ifndef AMGITHUBISSUEFAMILYVIEW_H
#define AMGITHUBISSUEFAMILYVIEW_H

#include <QWidget>

#include "AMGitHubIssueFamily.h"

class AMGitHubIssueFamilyView : public QWidget
{
Q_OBJECT
public:
	AMGitHubIssueFamilyView(AMGitHubIssueFamily *issueFamily, QWidget *parent = 0);

protected:
	AMGitHubIssueFamily *issueFamily_;
};

#endif // AMGITHUBISSUEFAMILYVIEW_H
