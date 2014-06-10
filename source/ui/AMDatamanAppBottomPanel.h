#ifndef AMDATAMANAPPBOTTOMPANEL_H
#define AMDATAMANAPPBOTTOMPANEL_H

#include "ui/AMBottomPanel.h"
#include "ui/AMStatusView.h"

#include <QToolButton>

/// This class implements the bottom panel used for the dataman application.  It adds the ability to add experiments and show the status view.
class AMDatamanAppBottomPanel : public AMBottomPanel
{
	Q_OBJECT

public:
	/// Constructor.
 	virtual ~AMDatamanAppBottomPanel();
	AMDatamanAppBottomPanel(QWidget *parent = 0);

signals:
	/// Signal used to tell the dataview that it should add an experiment.
	void addExperimentButtonClicked();

protected:
	/// The status view.
	AMStatusView *statusView_;
};

#endif // AMDATAMANAPPBOTTOMPANEL_H
