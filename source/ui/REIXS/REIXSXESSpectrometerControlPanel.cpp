#include "REIXSXESSpectrometerControlPanel.h"
#include <QGroupBox>
#include <QBoxLayout>
#include <QtGui/QFormLayout>
#include "ui/AMTopFrame.h"




REIXSXESSpectrometerControlPanel::REIXSXESSpectrometerControlPanel(REIXSXESMCPDetector* detector, QWidget *parent) :
    QWidget(parent)
{
	spectrometerEditor_ = new REIXSXESSpectrometerControlEditor(REIXSBeamline::bl()->spectrometer());
	detectorView_ = new REIXSXESMCPDetectorView(detector);
	sampleXUp_ = new AMControlMoveButton();
	sampleXDown_ = new AMControlMoveButton();

	QIcon iconUp, iconDown;
	iconUp.addFile(QString::fromUtf8(":/22x22/go-up-dark.png"), QSize(), QIcon::Normal, QIcon::Off);
	iconDown.addFile(QString::fromUtf8(":/22x22/go-down-dark.png"), QSize(), QIcon::Normal, QIcon::Off);

	REIXSSampleChamber* chamber = REIXSBeamline::bl()->sampleChamber();
	sampleXUp_->setControl(chamber->x());
	sampleXUp_->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10);
	sampleXUp_->setStepSizeIndex(1);
	sampleXUp_->setDirectionReversed(true);
	sampleXUp_->setIcon(iconDown); //sampleXUp increase the x step position, but moves the image down the detector view
	sampleXUp_->setIconSize(QSize(22, 22));
	sampleXUp_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);


	sampleXDown_->setControl(chamber->x());
	sampleXDown_->setStepSizes(QList<double>() << 0.2 << 1 << 5 << 10);
	sampleXDown_->setStepSizeIndex(1);
	sampleXDown_->setIcon(iconUp); //sampleXDown decrease the x step position, but moves the image up the detector view
	sampleXDown_->setIconSize(QSize(22, 22));
	sampleXDown_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);



	QGroupBox* detectorPreviewBox = new QGroupBox("Detector Preview");
	detectorPreviewBox->setLayout(new QVBoxLayout());
	detectorPreviewBox->layout()->addWidget(detectorView_);

	QVBoxLayout* outerVLayout = new QVBoxLayout();
	outerVLayout->setContentsMargins(0,0,0,0);
	outerVLayout->setSpacing(0);
	outerVLayout->addWidget(new AMTopFrame("XES Spectrometer Setup", QIcon(":/utilities-system-monitor.png")));

	QFormLayout* sampleAlignmentLayout = new QFormLayout();
	sampleAlignmentLayout->addRow("Shift Sample Upstream: ", sampleXDown_);
	sampleAlignmentLayout->addRow("Shift Sample Downstream: ", sampleXUp_);

	QHBoxLayout* hl = new QHBoxLayout();
	hl->setContentsMargins(12,12,12,12);
	hl->setSpacing(6);
	QVBoxLayout* vl1 = new QVBoxLayout();
	vl1->addStretch();
	vl1->addWidget(spectrometerEditor_);
	vl1->addLayout(sampleAlignmentLayout);
	vl1->addStretch();
	hl->addWidget(detectorPreviewBox);
	hl->addLayout(vl1);

	outerVLayout->addLayout(hl);
	setLayout(outerVLayout);

}
