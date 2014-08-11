/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "IDEASPersistentView.h"

#include "beamline/IDEAS/IDEASBeamline.h"
#include "ui/IDEAS/IDEASScalerView.h"


#include "ui/beamline/AMExtendedControlEditor.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QProgressBar>
#include <QGridLayout>
#include <QInputDialog>
#include <QDebug>

//#ifdef AM_MOBILITY_VIDEO_ENABLED
//#include "ui/AMBeamlineCameraWidgetWithSourceTabs.h"
//#endif

IDEASPersistentView::IDEASPersistentView(QWidget *parent) :
    QWidget(parent)
{
    beamStatusLabel_ = new QLabel("Beam is off!");

    beamOnButton_ = new QPushButton("Beam On");
    connect(beamOnButton_, SIGNAL(clicked()), this, SLOT(onBeamOnClicked()));

    beamOffButton_ = new QPushButton("Beam Off");
    connect(beamOffButton_, SIGNAL(clicked()), this, SLOT(onBeamOffClicked()));

    connect(IDEASBeamline::ideas(), SIGNAL(overallShutterStatus(bool)), this, SLOT(onShutterStatusChanged(bool)));

    calibrateButton_ = new QPushButton("Calibrate Energy");
    connect(calibrateButton_, SIGNAL(clicked()), this, SLOT(onCalibrateClicked()));

    ringCurrent_ = new QLabel("     mA");
    ringCurrent_->setAlignment(Qt::AlignCenter);
    ringCurrent_->setFont(QFont("Lucida Grande", 16, QFont::Bold));

    energyControlEditor_ = new AMExtendedControlEditor(IDEASBeamline::ideas()->monoEnergyControl());
    energyControlEditor_->setMaximumWidth(200);
    energyControlEditor_->setControlFormat('f', 1);

    monoCrystal_ = new QLabel("Crystal");
    monoCrystal_->setAlignment(Qt::AlignCenter);
    monoEnergyRange_ = new QLabel(" eV - eV");
    monoEnergyRange_->setAlignment(Qt::AlignCenter);

    IOldLabel_ = new QLabel("Old");
    I0Label_ = new QLabel("I_0");
    ISampleLabel_ = new QLabel("Sample");
    IReferenceLabel_ = new QLabel("Reference");

    IOldValueLabel_ = new QLabel("0");
    I0ValueLabel_ = new QLabel("0");
    ISampleValueLabel_ = new QLabel("0");
    IReferenceValueLabel_ = new QLabel("0");

    IOldBar_ = new QProgressBar;
    IOldBar_->setTextVisible(false);
    IOldBar_->setRange(100,145);


    I0Bar_ = new QProgressBar;
    I0Bar_->setTextVisible(false);
    I0Bar_->setRange(100,145);

    ISampleBar_ = new QProgressBar;
    ISampleBar_->setTextVisible(false);
    ISampleBar_->setRange(100,145);

    IReferenceBar_ = new QProgressBar;
    IReferenceBar_->setTextVisible(false);
    IReferenceBar_->setRange(100,145);

    QIcon iconUp, iconDown, iconLeft, iconRight, iconVerticalClose, iconVerticalOpen, iconHorizontalClose, iconHorizontalOpen;
    iconUp.addFile(QString::fromUtf8(":/22x22/go-up-dark.png"), QSize(), QIcon::Normal, QIcon::Off);
    iconDown.addFile(QString::fromUtf8(":/22x22/go-down-dark.png"), QSize(), QIcon::Normal, QIcon::Off);
    iconLeft.addFile(QString::fromUtf8(":/22x22/go-previous-dark.png"), QSize(), QIcon::Normal, QIcon::Off);
    iconRight.addFile(QString::fromUtf8(":/22x22/go-next-dark.png"), QSize(), QIcon::Normal, QIcon::Off);
    iconVerticalClose.addFile(QString::fromUtf8(":/22x22/go-close-vert-dark.png"), QSize(), QIcon::Normal, QIcon::Off);
    iconVerticalOpen.addFile(QString::fromUtf8(":/22x22/go-open-vert-dark.png"), QSize(), QIcon::Normal, QIcon::Off);
    iconHorizontalClose.addFile(QString::fromUtf8(":/22x22/go-close-horiz-dark.png"), QSize(), QIcon::Normal, QIcon::Off);
    iconHorizontalOpen.addFile(QString::fromUtf8(":/22x22/go-open-horiz-dark.png"), QSize(), QIcon::Normal, QIcon::Off);


    jjSlitsVertClose_ = new AMControlMoveButton();
    jjSlitsVertClose_->setControl(IDEASBeamline::ideas()->jjSlitVGap());
    jjSlitsVertClose_->setStepSizes(QList<double>() << 0.1 << 0.5 << 1 << 5);
    jjSlitsVertClose_->setStepSizeIndex(1);
    jjSlitsVertClose_->setDirectionReversed(true);
    jjSlitsVertClose_->setIcon(iconVerticalClose);
    jjSlitsVertClose_->setIconSize(QSize(22, 22));
    jjSlitsVertClose_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    jjSlitsVertClose_->setMinimumWidth(60);
    jjSlitsVertClose_->setMinimumHeight(60);

    jjSlitsVertOpen_ = new AMControlMoveButton();
    jjSlitsVertOpen_->setControl(IDEASBeamline::ideas()->jjSlitVGap());
    jjSlitsVertOpen_->setStepSizes(QList<double>() << 0.1 << 0.5 << 1 << 5);
    jjSlitsVertOpen_->setStepSizeIndex(1);
    //jjSlitsVertOpen_->setDirectionReversed(true);
    jjSlitsVertOpen_->setIcon(iconVerticalOpen);
    jjSlitsVertOpen_->setIconSize(QSize(22, 22));
    jjSlitsVertOpen_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    jjSlitsVertOpen_->setMinimumWidth(60);
    jjSlitsVertOpen_->setMinimumHeight(60);

    jjSlitsHorizClose_ = new AMControlMoveButton();
    jjSlitsHorizClose_->setControl(IDEASBeamline::ideas()->jjSlitHGap());
    jjSlitsHorizClose_->setStepSizes(QList<double>() << 0.1 << 0.5 << 1 << 5);
    jjSlitsHorizClose_->setStepSizeIndex(1);
    jjSlitsHorizClose_->setDirectionReversed(true);
    jjSlitsHorizClose_->setIcon(iconHorizontalClose);
    jjSlitsHorizClose_->setIconSize(QSize(22, 22));
    jjSlitsHorizClose_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    jjSlitsHorizClose_->setMinimumWidth(60);
    jjSlitsHorizClose_->setMinimumHeight(60);

    jjSlitsHorizOpen_ = new AMControlMoveButton();
    jjSlitsHorizOpen_->setControl(IDEASBeamline::ideas()->jjSlitHGap());
    jjSlitsHorizOpen_->setStepSizes(QList<double>() << 0.1 << 0.5 << 1 << 5);
    jjSlitsHorizOpen_->setStepSizeIndex(1);
    //jjSlitsHorOpen_->setDirectionReversed(true);
    jjSlitsHorizOpen_->setIcon(iconHorizontalOpen);
    jjSlitsHorizOpen_->setIconSize(QSize(22, 22));
    jjSlitsHorizOpen_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    jjSlitsHorizOpen_->setMinimumWidth(60);
    jjSlitsHorizOpen_->setMinimumHeight(60);

    jjSlitsUp_ = new AMControlMoveButton();
    jjSlitsUp_->setControl(IDEASBeamline::ideas()->jjSlitVCenter());
    jjSlitsUp_->setStepSizes(QList<double>() << 0.1 << 0.5 << 1 << 5);
    jjSlitsUp_->setStepSizeIndex(1);
    //jjSlitsUp_->setDirectionReversed(true);
    jjSlitsUp_->setIcon(iconUp);
    jjSlitsUp_->setIconSize(QSize(22, 22));
    jjSlitsUp_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    //jjSlitsUp_->setText("Up");
    jjSlitsUp_->setMinimumWidth(60);
    jjSlitsUp_->setMinimumHeight(60);


    jjSlitsDown_ = new AMControlMoveButton();
    jjSlitsDown_->setControl(IDEASBeamline::ideas()->jjSlitVCenter());
    jjSlitsDown_->setStepSizes(QList<double>() << 0.1 << 0.5 << 1 << 5);
    jjSlitsDown_->setStepSizeIndex(1);
    jjSlitsDown_->setDirectionReversed(true);
    jjSlitsDown_->setIcon(iconDown);
    jjSlitsDown_->setIconSize(QSize(22, 22));
    jjSlitsDown_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    //jjSlitsDown_->setText("Down");
    jjSlitsDown_->setMinimumWidth(60);
    jjSlitsDown_->setMinimumHeight(60);

    jjSlitsRight_ = new AMControlMoveButton();
    jjSlitsRight_->setControl(IDEASBeamline::ideas()->jjSlitHCenter());
    jjSlitsRight_->setStepSizes(QList<double>() << 0.1 << 0.5 << 1 << 5);
    jjSlitsRight_->setStepSizeIndex(1);
    jjSlitsRight_->setDirectionReversed(true);
    jjSlitsRight_->setIcon(iconRight);
    jjSlitsRight_->setIconSize(QSize(22, 22));
    jjSlitsRight_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    //jjSlitsRight_->setText("Right");
    jjSlitsRight_->setMinimumWidth(60);
    jjSlitsRight_->setMinimumHeight(60);

    jjSlitsLeft_ = new AMControlMoveButton();
    jjSlitsLeft_->setControl(IDEASBeamline::ideas()->jjSlitHCenter());
    jjSlitsLeft_->setStepSizes(QList<double>() << 0.1 << 0.5 << 1 << 5);
    jjSlitsLeft_->setStepSizeIndex(1);
    //jjSlitsLeft_->setDirectionReversed(true);
    jjSlitsLeft_->setIcon(iconLeft);
    jjSlitsLeft_->setIconSize(QSize(22, 22));
    jjSlitsLeft_->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    //jjSlitsLeft_->setText("Left");
    jjSlitsLeft_->setMinimumWidth(60);
    jjSlitsLeft_->setMinimumHeight(60);


    connect(IDEASBeamline::bl()->exposedControlByName("ringCurrent"), SIGNAL(valueChanged(double)), this, SLOT(onRingCurrentChanged(double)));

    connect(IDEASBeamline::ideas()->monoLowEV(),   SIGNAL(valueChanged(double)), this, SLOT(onCrystalChanged()));
    connect(IDEASBeamline::ideas()->monoHighEV(),  SIGNAL(valueChanged(double)), this, SLOT(onCrystalChanged()));
    connect(IDEASBeamline::ideas()->monoCrystal(), SIGNAL(valueChanged(double)), this, SLOT(onCrystalChanged()));

    connect(IDEASBeamline::ideas()->monoLowEV(),   SIGNAL(connected(bool)), this, SLOT(onCrystalChanged()));
    connect(IDEASBeamline::ideas()->monoHighEV(),  SIGNAL(connected(bool)), this, SLOT(onCrystalChanged()));
    connect(IDEASBeamline::ideas()->monoCrystal(), SIGNAL(connected(bool)), this, SLOT(onCrystalChanged()));




    QHBoxLayout *beamChangeLayout = new QHBoxLayout;
    beamChangeLayout->addWidget(beamOnButton_);
    beamChangeLayout->addWidget(beamOffButton_);

    QVBoxLayout *mainPanelLayout = new QVBoxLayout;
    mainPanelLayout->addWidget(ringCurrent_);
    mainPanelLayout->addLayout(beamChangeLayout);
    mainPanelLayout->addWidget(beamStatusLabel_, 0, Qt::AlignCenter);
    mainPanelLayout->addWidget(energyControlEditor_);
    mainPanelLayout->addWidget(calibrateButton_);
    mainPanelLayout->addWidget(monoCrystal_);
    mainPanelLayout->addWidget(monoEnergyRange_);
    mainPanelLayout->addStretch();

//#ifdef AM_MOBILITY_VIDEO_ENABLED
//	AMBeamlineCameraWidgetWithSourceTabs *cameraWidget;

//	QVBoxLayout *cameraPanelLayout = new QVBoxLayout;
//	cameraWidget = new AMBeamlineCameraWidgetWithSourceTabs(QUrl("http://v2e1608-102.clsi.ca/mjpg/2/video.mjpg"),"Sample 1",0,false);
//	cameraWidget->addSource("Sample 2", QUrl("http://v2e1608-102.clsi.ca/mjpg/3/video.mjpg"));
//	cameraWidget->addSource("Vacuum", QUrl("http://v2e1608-102.clsi.ca/mjpg/1/video.mjpg"));
//	cameraWidget->addSource("POE", QUrl("http://v2e1608-102.clsi.ca/mjpg/4/video.mjpg"));
//	cameraPanelLayout->addWidget(cameraWidget);
//#endif

    QGroupBox *jjSlitsPanel = new QGroupBox("Sample Slits");
    QGridLayout *jjSlitsButtons = new QGridLayout;
    jjSlitsButtons->addWidget(jjSlitsVertOpen_,0,0);
    jjSlitsButtons->addWidget(jjSlitsVertClose_,1,0);
    jjSlitsButtons->addWidget(jjSlitsHorizOpen_,0,1);
    jjSlitsButtons->addWidget(jjSlitsHorizClose_,1,1);
    jjSlitsButtons->addWidget(jjSlitsUp_,0,3);
    jjSlitsButtons->addWidget(jjSlitsDown_,1,3);
    jjSlitsButtons->addWidget(jjSlitsLeft_,0,4);
    jjSlitsButtons->addWidget(jjSlitsRight_,1,4);
    jjSlitsPanel->setLayout(jjSlitsButtons);


    QVBoxLayout *scalerPanelLayout = new QVBoxLayout;
    scalerPanelLayout->addWidget(new IDEASScalerView());

    QGridLayout *detectorPanelLayout = new QGridLayout;
    detectorPanelLayout->addWidget(IOldLabel_,0,0);
    detectorPanelLayout->addWidget(IOldBar_,0,1,1,2);
    detectorPanelLayout->addWidget(IOldValueLabel_,0,3,1,2);
    detectorPanelLayout->addWidget(I0Label_,1,0);
    detectorPanelLayout->addWidget(I0Bar_,1,1,1,2);
    detectorPanelLayout->addWidget(I0ValueLabel_,1,3,1,2);
    detectorPanelLayout->addWidget(ISampleLabel_,2,0);
    detectorPanelLayout->addWidget(ISampleBar_,2,1,1,2);
    detectorPanelLayout->addWidget(ISampleValueLabel_,2,3,1,2);
    detectorPanelLayout->addWidget(IReferenceLabel_,3,0);
    detectorPanelLayout->addWidget(IReferenceBar_,3,1,1,2);
    detectorPanelLayout->addWidget(IReferenceValueLabel_,3,3,1,2);


    QGroupBox *persistentPanel = new QGroupBox("IDEAS Beamline");
    persistentPanel->setLayout(mainPanelLayout);



    QGroupBox *scalerPanel = new QGroupBox("Preamp Settings");
    scalerPanel->setLayout(scalerPanelLayout);

    QGroupBox *detectorPanel = new QGroupBox("Ion Chamber Currents");
    detectorPanel->setLayout(detectorPanelLayout);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(persistentPanel);

//    #ifdef AM_MOBILITY_VIDEO_ENABLED
//    QGroupBox *cameraPanel = new QGroupBox("Camera Settings");
//    cameraPanel->setLayout(cameraPanelLayout);
//    layout->addWidget(cameraPanel);
//    #endif

    layout->addWidget(jjSlitsPanel);
    layout->addWidget(scalerPanel);
    //layout->addWidget(detectorPanel);

    setLayout(layout);

    setMaximumWidth(400);
    setMinimumWidth(400);
}

void IDEASPersistentView::onBeamOnClicked()
{
    AMAction3 *beamOn = IDEASBeamline::ideas()->createBeamOnAction();
    beamOn->start();
}

void IDEASPersistentView::onBeamOffClicked()
{
    AMAction3 *beamOff = IDEASBeamline::ideas()->createBeamOffAction();
    beamOff->start();
}

void IDEASPersistentView::onShutterStatusChanged(bool state)
{
    beamOnButton_->setDisabled(state);
    beamOffButton_->setEnabled(state);
    beamStatusLabel_->setText(state ? "Beam is on!" : "Beam is off!");
}


void IDEASPersistentView::onOldCountsChanged()
{
    double value = 0;
//    IDEASBeamline::bl()->exposedDetectorByName("I_vac_6485")->data(&value);
    IOldBar_->setValue(int(200 + 10*log10(qAbs(value))));
    //IOldBar_->setValue(value/1000000);
    //qdebug() << "I_Old_bar" << int(200 + 10*log10(qAbs(value)));
    //qdebug() << "I_Old" << value << "log10(I_Old)" << log10(value) << log10(qAbs(value));
    IOldValueLabel_->setText(QString::number(value, 'e', 2));
    //IOldValueLabel_->setText(value);
}

void IDEASPersistentView::onI0CountsChanged()
{
    double value = 0;
    IDEASBeamline::bl()->exposedDetectorByName("I_0")->data(&value);
    //I0Bar_->setValue(int(200 + 10*log10(qAbs(value))));
    I0Bar_->setValue(int(value/1000000));
    //qdebug() << "I_0" << int(-3.25/log10(qAbs(value)));
    //I0ValueLabel_->setText(QString::number(value, 'e', 2));
    I0ValueLabel_->setText(QString("%1k").arg(QString::number(value/1000,'f',0)));

   // ui->signalI0Bar->setValue(int(counts*600./1.e6));
    //ui->signalI0Value->setText(QString::number(counts, 'e', 2));
}

void IDEASPersistentView::onSampleCountsChanged()
{
    double value = 0;
    IDEASBeamline::bl()->exposedDetectorByName("I_sample")->data(&value);
    //ISampleBar_->setValue(int(200 + 10*log10(qAbs(value))));
    ISampleBar_->setValue(int(value/1000000));
    //ISampleValueLabel_->setText(QString::number(value, 'e', 2));
    ISampleValueLabel_->setText(QString("%1k").arg(QString::number(value/1000,'f',0)));
}

void IDEASPersistentView::onReferenceCountsChanged()
{
    double value = 0;
    IDEASBeamline::bl()->exposedDetectorByName("I_ref")->data(&value);
    //IReferenceBar_->setValue(int(200 + 10*log10(qAbs(value))));
    IReferenceBar_->setValue(int(value/1000000));
    //IReferenceValueLabel_->setText(QString::number(value, 'e', 2));
    IReferenceValueLabel_->setText(QString("%1k").arg(QString::number(value/1000,'f',0)));
}

void IDEASPersistentView::onCrystalChanged()
{

    monoCrystal_->setText(IDEASBeamline::bl()->exposedControlByName("monoCrystal")->enumNameAt(IDEASBeamline::bl()->exposedControlByName("monoCrystal")->value()));
    monoEnergyRange_->setText(QString("%1 eV - %2 eV").arg(IDEASBeamline::ideas()->monoLowEV()->value()).arg(IDEASBeamline::ideas()->monoHighEV()->value()));

//    if(IDEASBeamline::ideas()->monoLowEV()->value() == -1 || IDEASBeamline::ideas()->monoHighEV()->value() == -1){
//	qdebug() << "Rechecking Energy Range";
//	crystalTimer_->start();
//    }
}

void IDEASPersistentView::onRingCurrentChanged(double current)
{
    ringCurrent_->setText(QString("%1 mA").arg(current));
    if(current < 10) ringCurrent_->setStyleSheet("QLabel { background-color : red; color : white}");
    else ringCurrent_->setStyleSheet("QLabel { color : black; }");


}



void IDEASPersistentView::onCalibrateClicked()
{
    //QInputDialog  *calibrateDialog_ = new QInputDialog();

    bool ok;
    double newE = QInputDialog::getDouble(this,"Monochromator Energy Calibration","Enter Current Calibrated Energy:",IDEASBeamline::ideas()->monoEnergyControl()->value(),IDEASBeamline::ideas()->monoLowEV()->value(),IDEASBeamline::ideas()->monoHighEV()->value(),1,&ok);


    if(ok)
    {
        double oldAngleOffset = IDEASBeamline::ideas()->monoAngleOffset()->value();
        double currentE = IDEASBeamline::ideas()->monoEnergyControl()->value();
        double mono2d = IDEASBeamline::ideas()->mono2d()->value();
        double braggAngle = IDEASBeamline::ideas()->monoBraggAngle()->value();

        double dE = newE - currentE;

        double angleDetla = -12398.4193 / (mono2d * currentE * currentE * cos(braggAngle * M_PI / 180)) * dE * 180 / M_PI;

        IDEASBeamline::ideas()->monoAngleOffset()->move(oldAngleOffset + angleDetla);
	//qdebug() << newE << oldAngleOffset << currentE << mono2d << braggAngle << dE << angleDetla << oldAngleOffset + angleDetla;


    }

}
