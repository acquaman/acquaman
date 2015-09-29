#include "REIXSSampleManagementPre2013Widget.h"

#include <QGridLayout>
#include <QUrl>
#include <QGroupBox>
#include <QSpacerItem>

#include "ui/AMTopFrame.h"

#include "ui/AMBeamlineCameraWidget.h"

REIXSSampleManagementPre2013Widget::REIXSSampleManagementPre2013Widget(QWidget *manipulatorWidget, const QUrl& sampleCamera1Url, const QUrl& sampleCamera2Url, AMSamplePlatePre2013* samplePlate, AMSampleManipulator* manipulator, QWidget *parent) :
	QWidget(parent)
{
#ifdef AM_MOBILITY_VIDEO_ENABLED




	cameraWidget1_ = new AMBeamlineCameraWidget(this, false);
	cameraWidget1_->playSource(sampleCamera1Url);
	cameraWidget1_->setCrosshairCenterPosition(QPointF(0.477193, 0.64924));
	cameraWidget1_->setCrosshairPosition(QPointF(0.477193, 0.64924));
	cameraWidget1_->setCrosshairVisible(true);
	cameraWidget1_->setCrosshairLocked(true);
	cameraWidget1_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);


	cameraWidget2_ = new AMBeamlineCameraWidget(this, false);
	cameraWidget2_->playSource(sampleCamera2Url);
	cameraWidget2_->setCrosshairCenterPosition(QPointF(0.587617, 0.551402));
	cameraWidget2_->setCrosshairPosition(QPointF(0.587617, 0.551402));
	cameraWidget2_->setCrosshairVisible(true);
	cameraWidget2_->setCrosshairLocked(true);
	cameraWidget2_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);

#else
	Q_UNUSED(sampleCamera1Url);
	Q_UNUSED(sampleCamera2Url);
#endif

	topFrame_ = new AMTopFrame("Sample Management & Positioning", QIcon(":/system-software-update.png"));

	plateView_ = new AMSamplePlatePre2013View(samplePlate);
	plateView_->setManipulator(manipulator);

	manipulatorWidget_ = manipulatorWidget;

	QSpacerItem *bottomSpacer = new QSpacerItem(1,50,QSizePolicy::MinimumExpanding,QSizePolicy::Minimum);

	connect(plateView_, SIGNAL(newSamplePlateSelected()), this, SLOT(onNewSamplePlateSelected()));

	QVBoxLayout *vl = new QVBoxLayout();
	QGridLayout* gl = new QGridLayout();
	vl->addWidget(topFrame_);
	vl->addLayout(gl);

#ifdef AM_MOBILITY_VIDEO_ENABLED
	gl->addWidget(cameraWidget1_, 0, 0, 1, 2);
	gl->addWidget(cameraWidget2_, 0, 2, 1, 2);
#endif
	gl->addWidget(plateView_, 0, 4, 3, 1);
	gl->addWidget(manipulatorWidget_, 1, 1, 1, 2);
	gl->setColumnStretch(0,1);
	gl->setColumnStretch(1,2);
	gl->setColumnStretch(2,2);
	gl->setColumnStretch(3,1);
	gl->setColumnStretch(4,0);

	gl->setRowStretch(0,1);
	gl->setRowStretch(1,0);
	gl->setRowStretch(2,0);

	gl->addItem(bottomSpacer,2,1,1,4);

	vl->setContentsMargins(0,0,0,0);
	gl->setContentsMargins(10, 0, 10, 0);

	setLayout(vl);
}

REIXSSampleManagementPre2013Widget::~REIXSSampleManagementPre2013Widget(){}

void REIXSSampleManagementPre2013Widget::onNewSamplePlateSelected() {
	emit newSamplePlateSelected(plateView_->samplePlate());
}
