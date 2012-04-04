#include "AMWorkflowView3.h"
#include "actions3/AMActionRunner3.h"

#include "util/AMFontSizes.h"

#include <QBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>

AMWorkflowView3::AMWorkflowView3(QWidget *parent) :
    QWidget(parent)
{
	layoutSpacer_ = 0;

	QHBoxLayout* hl = new QHBoxLayout();
	hl->setContentsMargins(12,12,12,12);
	hl->setSpacing(12);

	QLabel* titleIcon = new QLabel();
	titleIcon->setMaximumSize(36,36);
	titleIcon->setPixmap(QPixmap(":/user-away.png"));
	titleIcon->setScaledContents(true);
	hl->addWidget(titleIcon);
	QLabel* titleLabel = new QLabel("Workflow");
	titleLabel->setStyleSheet("font: " AM_FONT_XLARGE_ "pt \"Lucida Grande\";\ncolor: rgb(79, 79, 79);");
	hl->addWidget(titleLabel);
	hl->addStretch(1);
	addActionButton_ = new QPushButton("Add Action...");
	hl->addWidget(addActionButton_);

    currentView_ = new AMActionRunnerCurrentView3(AMActionRunner3::s());
    queueView_ = new AMActionRunnerQueueView3(AMActionRunner3::s());
    historyView_ = new AMActionHistoryView3(AMActionRunner3::s());
	addActionDialog_ = 0;

	QVBoxLayout* vl = new QVBoxLayout(this);
	vl->setContentsMargins(0,0,0,0);
	vl->setSpacing(0);

	vl->addLayout(hl);
	vl->addWidget(historyView_);
	vl->addSpacing(12);
	vl->addWidget(currentView_);
	vl->addSpacing(12);
	vl->addWidget(queueView_);

	connect(queueView_, SIGNAL(collapsed(bool)), this, SLOT(onViewCollapsed()));
	connect(historyView_, SIGNAL(collapsed(bool)), this, SLOT(onViewCollapsed()));
	connect(addActionButton_, SIGNAL(clicked()), this, SLOT(onAddActionButtonClicked()));
}


#include <QSpacerItem>
void AMWorkflowView3::onViewCollapsed()
{
	// both collapsed but we don't have a spacer? Add it to bottom of layout to keep layout from spreading everything out.
	if(historyView_->isCollapsed() && queueView_->isCollapsed() && !layoutSpacer_) {
		layoutSpacer_ = new QSpacerItem(0,0,QSizePolicy::Minimum,QSizePolicy::Expanding);
		layout()->addItem(layoutSpacer_);
	}

	// At least one not collapsed, but we do have a spacer? Get rid of it.
	else if((!historyView_->isCollapsed() || !queueView_->isCollapsed()) && layoutSpacer_) {
		layout()->removeItem(layoutSpacer_);
		QSpacerItem* spacer = layoutSpacer_;
		layoutSpacer_ = 0;
		delete spacer;
	}
}

void AMWorkflowView3::onAddActionButtonClicked()
{
	if(!addActionDialog_) {
        addActionDialog_ = new AMAddActionDialog3(this);
	}
	addActionDialog_->show();
}
