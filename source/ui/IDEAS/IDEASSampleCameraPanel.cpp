#include "beamline/IDEAS/IDEASBeamline.h"
#include "IDEASSampleCameraPanel.h"
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

    QIcon iconUp, iconDown, iconLeft, iconRight;
    iconUp.addFile(QString::fromUtf8(":/22x22/go-up-dark.png"), QSize(), QIcon::Normal, QIcon::Off);
    iconDown.addFile(QString::fromUtf8(":/22x22/go-down-dark.png"), QSize(), QIcon::Normal, QIcon::Off);
    iconLeft.addFile(QString::fromUtf8(":/22x22/go-previous-dark.png"), QSize(), QIcon::Normal, QIcon::Off);
    iconRight.addFile(QString::fromUtf8(":/22x22/go-next-dark.png"), QSize(), QIcon::Normal, QIcon::Off);


    sampleUp_ = new AMControlMoveButton();
    sampleUp_->setControl(IDEASBeamline::ideas()->samplePlatformVertical());
    sampleUp_->setStepSizes(QList<double>() << 0.5 << 1 << 5);
    sampleUp_->setStepSizeIndex(1);
    //sampleUp_->setDirectionReversed(true);
    sampleUp_->setIcon(iconUp);
    sampleUp_->setIconSize(QSize(22, 22));
    sampleUp_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    //sampleUp_->setText("Up");
    sampleUp_->setMinimumWidth(60);
    sampleUp_->setMinimumHeight(60);


    sampleDown_ = new AMControlMoveButton();
    sampleDown_->setControl(IDEASBeamline::ideas()->samplePlatformVertical());
    sampleDown_->setStepSizes(QList<double>() << 0.5 << 1 << 5);
    sampleDown_->setStepSizeIndex(1);
    sampleDown_->setDirectionReversed(true);
    sampleDown_->setIcon(iconDown);
    sampleDown_->setIconSize(QSize(22, 22));
    sampleDown_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    //sampleDown_->setText("Down");
    sampleDown_->setMinimumWidth(60);
    sampleDown_->setMinimumHeight(60);

    sampleInboard_ = new AMControlMoveButton();
    sampleInboard_->setControl(IDEASBeamline::ideas()->samplePlatformHorizontal());
    sampleInboard_->setStepSizes(QList<double>() << 0.5 << 1 << 5);
    sampleInboard_->setStepSizeIndex(1);
    //sampleInboard_->setDirectionReversed(true);
    sampleInboard_->setIcon(iconRight);
    sampleInboard_->setIconSize(QSize(22, 22));
    sampleInboard_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    //sampleInboard_->setText("Right");
    sampleInboard_->setMinimumWidth(60);
    sampleInboard_->setMinimumHeight(60);

    sampleOutboard_ = new AMControlMoveButton();
    sampleOutboard_->setControl(IDEASBeamline::ideas()->samplePlatformHorizontal());
    sampleOutboard_->setStepSizes(QList<double>() << 0.5 << 1 << 5);
    sampleOutboard_->setStepSizeIndex(1);
    sampleOutboard_->setDirectionReversed(true);
    sampleOutboard_->setIcon(iconLeft);
    sampleOutboard_->setIconSize(QSize(22, 22));
    sampleOutboard_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    //sampleOutboard_->setText("Left");
    sampleOutboard_->setMinimumWidth(60);
    sampleOutboard_->setMinimumHeight(60);

    vacuumStageUp_ = new AMControlMoveButton();
    vacuumStageUp_->setControl(IDEASBeamline::ideas()->vacuumSampleStage());
    vacuumStageUp_->setStepSizes(QList<double>() << 0.5 << 1 << 5);
    vacuumStageUp_->setStepSizeIndex(1);
    vacuumStageUp_->setDirectionReversed(true);
    vacuumStageUp_->setIcon(iconUp);
    vacuumStageUp_->setIconSize(QSize(22, 22));
    vacuumStageUp_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    //vacuumStageUp_->setText("Up");
    vacuumStageUp_->setMinimumWidth(60);
    vacuumStageUp_->setMinimumHeight(60);

    vacuumStageDown_ = new AMControlMoveButton();
    vacuumStageDown_->setControl(IDEASBeamline::ideas()->vacuumSampleStage());
    vacuumStageDown_->setStepSizes(QList<double>() << 0.5 << 1 << 5);
    vacuumStageDown_->setStepSizeIndex(1);
    //vacuumStageDown_->setDirectionReversed(true);
    vacuumStageDown_->setIcon(iconDown);
    vacuumStageDown_->setIconSize(QSize(22, 22));
    vacuumStageDown_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    //vacuumStageDown_->setText("Down");
    vacuumStageDown_->setMinimumWidth(60);
    vacuumStageDown_->setMinimumHeight(60);




#ifdef AM_MOBILITY_VIDEO_ENABLED
	AMBeamlineCameraWidgetWithSourceTabs *cameraWidget;

	QGridLayout *cameraPanelLayout = new QGridLayout;
	cameraWidget = new AMBeamlineCameraWidgetWithSourceTabs(QUrl("http://v2e1608-102.clsi.ca/mjpg/2/video.mjpg"),"Sample 1",0,false);
	cameraWidget->addSource("Sample 2", QUrl("http://v2e1608-102.clsi.ca/mjpg/3/video.mjpg"));
	cameraWidget->addSource("Vacuum", QUrl("http://v2e1608-102.clsi.ca/mjpg/1/video.mjpg"));
	cameraWidget->addSource("POE", QUrl("http://v2e1608-102.clsi.ca/mjpg/4/video.mjpg"));
	cameraPanelLayout->addWidget(cameraWidget,0,0,3,1);
#endif

	QTabWidget *sampleStageTabs = new QTabWidget;
	sampleStageTabs->setMaximumWidth(250);

	QGroupBox *airStageGroup = new QGroupBox;
	QGridLayout *airStageButtons = new QGridLayout;
	airStageButtons->addWidget(sampleUp_,0,1);
	airStageButtons->addWidget(sampleDown_,2,1);
	airStageButtons->addWidget(sampleInboard_,1,2);
	airStageButtons->addWidget(sampleOutboard_,1,0);
	airStageGroup->setLayout(airStageButtons);
	airStageGroup->setMaximumHeight(200);
	airStageGroup->setTitle("Stage Position");
	sampleStageTabs->addTab(airStageGroup,"Air Stage");

	QGroupBox *vacuumStageGroup = new QGroupBox;
	QGridLayout *vacuumStageButtons = new QGridLayout;
	vacuumStageButtons->addWidget(vacuumStageUp_,0,1);
	vacuumStageButtons->addWidget(vacuumStageDown_,2,1);
	vacuumStageGroup->setLayout(vacuumStageButtons);
	vacuumStageGroup->setMaximumHeight(200);
	vacuumStageGroup->setTitle("Stage Position");
	sampleStageTabs->addTab(vacuumStageGroup,"Vac Stage");

	cameraPanelLayout->addWidget(sampleStageTabs,2,1,1,1);


	setLayout(cameraPanelLayout);
}
