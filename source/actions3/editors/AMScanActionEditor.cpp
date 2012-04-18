#include "AMScanActionEditor.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStringBuilder>

#include "acquaman/AMScanConfiguration.h"
#include "ui/acquaman/AMScanConfigurationView.h"

AMScanActionEditor::AMScanActionEditor(AMScanActionInfo *info, QWidget *parent)
	: QFrame(parent)
{
	info_ = info;
	setFrameStyle(QFrame::StyledPanel);

	configView_ = 0;

	scanName_ = new QLabel(info_->config()->detailedDescription() % ": " % info_->config()->name());
	connect(info_->config(), SIGNAL(nameChanged(QString)), this, SLOT(onScanNameChanged()));

	QPushButton *configViewButton = new QPushButton(QIcon(":/32x32/hammer-wrench.png"), "Show Configuration");
	connect(configViewButton, SIGNAL(clicked()), this, SLOT(onScanConfigurationViewRequested()));

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(scanName_);
	layout->addStretch();
	layout->addWidget(configViewButton);

	setLayout(layout);
}

AMScanActionEditor::~AMScanActionEditor()
{
	if (configView_){

		delete configView_;
		configView_ = 0;
	}
}

void AMScanActionEditor::onScanConfigurationViewRequested()
{
	if (configView_)
		configView_->raise();

	else {

		configView_ = info_->config()->createView();
		configView_->show();
	}
}

void AMScanActionEditor::onScanNameChanged()
{
	scanName_->setText(info_->config()->detailedDescription() % ": " % info_->config()->name());
}
