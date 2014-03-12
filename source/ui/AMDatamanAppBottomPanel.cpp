#include "AMDatamanAppBottomPanel.h"

 AMDatamanAppBottomPanel::~AMDatamanAppBottomPanel(){}
AMDatamanAppBottomPanel::AMDatamanAppBottomPanel(QWidget *parent)
	: AMBottomPanel(parent)
{
	QToolButton *addExperimentButton = new QToolButton;
	addExperimentButton->setIcon(QIcon(":/22x22/folder-new.png"));
	addExperimentButton->setToolTip("Add Experiment Folder...");
	connect(addExperimentButton, SIGNAL(clicked()), this, SIGNAL(addExperimentButtonClicked()));

	statusView_ = new AMStatusView;

	layout_->addSpacing(25);
	layout_->addWidget(addExperimentButton);
	layout_->addStretch();
	layout_->addWidget(statusView_);
}
