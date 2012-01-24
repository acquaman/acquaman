#include "VESPERSWorkflowAssistantView.h"

#include <QHBoxLayout>

VESPERSWorkflowAssistantView::VESPERSWorkflowAssistantView(VESPERSWorkflowAssistant *assistant, QWidget *parent)
	: QWidget(parent)
{
	assistant_ = assistant;
//	setVisible(false);
//	connect(assistant_, SIGNAL(workflowRunningChanged(bool)), this, SLOT(setVisible(bool)));

	progressBar_ = new QProgressBar;
	progressBar_->setFixedWidth(400);

	totalScans_ = new QSpinBox;
	totalScans_->setMinimum(1);
	totalScans_->setValue(1);
	connect(totalScans_, SIGNAL(valueChanged(int)), this, SLOT(onTotalScansChanged(int)));

	name_ = new QLabel;

	currentScan_ = new QLabel;
	connect(assistant_, SIGNAL(currentScanChanged(int)), this, SLOT(onCurrentScanChanged(int)));

	connect(assistant_, SIGNAL(configurationChanged()), this, SLOT(onScanConfigurationChanged()));

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addStretch();
	layout->addWidget(name_);
	layout->addSpacing(25);
	layout->addWidget(progressBar_);
	layout->addSpacing(25);
	layout->addWidget(currentScan_);
	layout->addWidget(totalScans_);
	layout->addStretch();

	setLayout(layout);
}

void VESPERSWorkflowAssistantView::onTotalScansChanged(int num)
{
	if (num != assistant_->totalScans() && num > assistant_->currentScan())
		assistant_->setTotalScans(num);
}

void VESPERSWorkflowAssistantView::onCurrentScanChanged(int index)
{
	currentScan_->setText(QString("%1 /").arg(index));
	progressBar_->setValue(int(index*100/assistant_->totalScans()));
	totalScans_->setMinimum(index);
}

void VESPERSWorkflowAssistantView::onScanConfigurationChanged()
{
	name_->setText(assistant_->configName());
	progressBar_->setValue(0);
	totalScans_->setValue(assistant_->totalScans());
	totalScans_->setMinimum(1);
	currentScan_->setText("1 /");
}
