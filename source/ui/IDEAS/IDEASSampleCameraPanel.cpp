#include "beamline/IDEAS/IDEASBeamline.h"
#include "IDEASSampleCameraPanel.h"
#include "ui/AMMotorGroupView.h"
#include "ui/AMMotorGroupView.h"
#include <QGroupBox>
#include <QBoxLayout>
#include <QtGui/QFormLayout>
#include <QTabWidget>

#ifdef AM_MOBILITY_VIDEO_ENABLED
#include "ui/AMBeamlineCameraWidgetWithSourceTabs.h"
#endif


IDEASSampleCameraPanel::IDEASSampleCameraPanel(QWidget *parent) :
	QWidget(parent)
{

	sampleMotorGroupView_ = new AMMotorGroupObjectView(IDEASBeamline::ideas()->samplePlatformMotorGroupObject());
	sampleMotorGroupView_->setJogSize(1);
	sampleMotorGroupView_->setJogSingleStep(0.5);
	sampleMotorGroupView_->setJogPrecision(1);

	sampleMotorGroupView_->setMotorValuesPrecision(1);
	sampleMotorGroupView_->setMaximumWidth(250);

	vacuumMotorGroupView_ = new AMMotorGroupObjectView(IDEASBeamline::ideas()->vacuumStageMotorGroupObject());
	vacuumMotorGroupView_->setJogRange(0, 25);
	vacuumMotorGroupView_->setJogSize(1);
	vacuumMotorGroupView_->setJogSingleStep(0.5);
	vacuumMotorGroupView_->setJogPrecision(1);

	vacuumMotorGroupView_->setMotorValuesPrecision(1);
	vacuumMotorGroupView_->setMotorValuesRange(-10, 150);

	vacuumMotorGroupView_->setMaximumWidth(250);


	QVBoxLayout *cameraPanelLayout = new QVBoxLayout;
	QTabWidget *sampleStageTabs = new QTabWidget;
	QGroupBox *airStageGroup = new QGroupBox;
	QVBoxLayout *airStageLayout = new QVBoxLayout;
	QGroupBox *vacStageGroup = new QGroupBox;
	QVBoxLayout *vacStageLayout = new QVBoxLayout;
	QGroupBox *camera3Group = new QGroupBox;
	QVBoxLayout *camera3Layout = new QVBoxLayout;
	QGroupBox *camera4Group = new QGroupBox;
	QVBoxLayout *camera4Layout = new QVBoxLayout;


#ifdef AM_MOBILITY_VIDEO_ENABLED
	AMBeamlineCameraWidget *sampleCameraWidget;
	AMBeamlineCameraWidget *vacuumCameraWidget;
	AMBeamlineCameraWidget *camera3Widget;
	AMBeamlineCameraWidget *camera4Widget;

	sampleCameraWidget = new AMBeamlineCameraWidget(0,false);
	vacuumCameraWidget = new AMBeamlineCameraWidget(0,false);
	camera3Widget = new AMBeamlineCameraWidget(0,false);
	camera4Widget = new AMBeamlineCameraWidget(0,false);
	sampleCameraWidget->playSource(QUrl("http://v2e1608-102.clsi.ca/mjpg/1/video.mjpg"));
	vacuumCameraWidget->playSource(QUrl("http://v2e1608-102.clsi.ca/mjpg/2/video.mjpg"));
	camera3Widget->playSource(QUrl("http://v2e1608-102.clsi.ca/mjpg/3/video.mjpg"));
	camera4Widget->playSource(QUrl("http://v2e1608-102.clsi.ca/mjpg/4/video.mjpg"));

	airStageLayout->addWidget(sampleCameraWidget);
	vacStageLayout->addWidget(vacuumCameraWidget);
	camera3Layout->addWidget(camera3Widget);
	camera4Layout->addWidget(camera4Widget);
#endif



	airStageGroup->setLayout(airStageLayout);
	airStageLayout->addWidget(sampleMotorGroupView_);
	sampleStageTabs->addTab(airStageGroup,"Air Stage");

	vacStageGroup->setLayout(vacStageLayout);
	vacStageLayout->addWidget(vacuumMotorGroupView_);
	sampleStageTabs->addTab(vacStageGroup,"Vacuum Stage");

	camera3Group->setLayout(camera3Layout);
	sampleStageTabs->addTab(camera3Group, "Camera 3");

	camera4Group->setLayout(camera4Layout);
	sampleStageTabs->addTab(camera4Group, "Camera 4");

	cameraPanelLayout->addWidget(sampleStageTabs);

	setLayout(cameraPanelLayout);
}
