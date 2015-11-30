#include "SGMTestsWindow.h"

#include "tests/SGM/SGMEnergyPositionTest.h"
#include "tests/SGM/SGMEnergyPositionTestView.h"
#include "tests/SGM/SGMEnergyTrajectoryTestView.h"
#include "tests/SGM/SGMEnergyControlTestView.h"
#include "tests/SGM/AMScalerAnalyserTestView.h"
#include "tests/SGM/SGMGratingVelocityTests.h"
#include "tests/SGM/SGMUndulatorVelocityTests.h"
#include "qjson/serializer.h"

#include "beamline/SGM/energy/SGMEnergyPosition.h"
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

SGMTestsWindow::SGMTestsWindow(QWidget *parent) : QMainWindow(parent)
{
	energyTestView_ = 0;
	energyTrajectoryTestView_ = 0;
	energyControlTestView_ = 0;
	scalerAnalyserTestView_ = 0;
	setupUi();

	SGMEnergyPositionTest energyPositionTests;
	energyPositionTests.performTests();

	QStringList testResults = energyPositionTests.testResults();

	QString resultString;
	foreach(QString result, testResults) {
		resultString.append(QString("\n%1").arg(result));
	}

	outputTextEdit_->setText(resultString);
}

void SGMTestsWindow::onShowEnergyViewClicked()
{
	if(energyTestView_ == 0) {

		SGMEnergyPosition* testEnergyPosition =
				new SGMEnergyPosition(SGMGratingSupport::LowGrating,
						      -412460.94,
						      SGMUndulatorSupport::FirstHarmonic,
						      13.2265442,
						      0,
						      358.199551,
						      this);

		energyTestView_ = new SGMEnergyPositionTestView(testEnergyPosition);
	}

	energyTestView_->show();
	energyTestView_->raise();

}

void SGMTestsWindow::onShowTrajectoryViewClicked()
{

	if(energyTrajectoryTestView_ == 0) {

		energyTrajectoryTestView_ = new SGMEnergyTrajectoryTestView();
		energyTrajectoryTestView_->resize(1024,768);
	}
	energyTrajectoryTestView_->show();
	energyTrajectoryTestView_->raise();
}

void SGMTestsWindow::onShowEnergyControlViewClicked()
{
	if(energyControlTestView_ == 0) {

		energyControlTestView_ = new SGMEnergyControlTestView();
	}

	energyControlTestView_->show();
	energyControlTestView_->raise();

}

void SGMTestsWindow::onShowScalerAnalysisTestClicked()
{
	if(scalerAnalyserTestView_ == 0) {

		scalerAnalyserTestView_ = new AMScalerAnalyserTestView();
	}

	scalerAnalyserTestView_->show();
	scalerAnalyserTestView_->raise();
}

void SGMTestsWindow::onRunGratingVelocityTestClicked()
{
	SGMGratingVelocityTests* velocityTest = new SGMGratingVelocityTests();
}

void SGMTestsWindow::onRunUndulatorVelocityTestClicked()
{
	SGMUndulatorVelocityTests* undulatorTest = new SGMUndulatorVelocityTests();
}

void SGMTestsWindow::setupUi()
{


	QGroupBox* centralGroupBox = new QGroupBox();
	QHBoxLayout* mainLayout = new QHBoxLayout();
	QVBoxLayout* buttonsLayout = new QVBoxLayout();
	QGroupBox* resultsGroupBox = new QGroupBox("Test Results");
	QHBoxLayout* resultsLayout = new QHBoxLayout();

	centralGroupBox->setLayout(mainLayout);
	mainLayout->addWidget(resultsGroupBox);
	mainLayout->addLayout(buttonsLayout);
	resultsGroupBox->setLayout(resultsLayout);

	setCentralWidget(centralGroupBox);

	showEnergyViewButton_ = new QPushButton("Show Energy Test View");
	connect(showEnergyViewButton_, SIGNAL(clicked(bool)), this, SLOT(onShowEnergyViewClicked()));
	buttonsLayout->addWidget(showEnergyViewButton_);

	showTrajectoryViewButton_ = new QPushButton("Show Energy Trajectory Test View");
	connect(showTrajectoryViewButton_, SIGNAL(clicked(bool)), this, SLOT(onShowTrajectoryViewClicked()));
	buttonsLayout->addWidget(showTrajectoryViewButton_);

	showEnergyControlViewButton_ = new QPushButton("Show Energy Control Test View");
	connect(showEnergyControlViewButton_, SIGNAL(clicked(bool)), this, SLOT(onShowEnergyControlViewClicked()));
	buttonsLayout->addWidget(showEnergyControlViewButton_);

	showScalerAnalysisViewButton_ = new QPushButton("Show Scaler Analysis Test View");
	connect(showScalerAnalysisViewButton_, SIGNAL(clicked()), this, SLOT(onShowScalerAnalysisTestClicked()));
	buttonsLayout->addWidget(showScalerAnalysisViewButton_);

	runGratingVelocityTest_ = new QPushButton("Run Grating Velocity Tests");
	connect(runGratingVelocityTest_, SIGNAL(clicked(bool)), this, SLOT(onRunGratingVelocityTestClicked()));
	buttonsLayout->addWidget(runGratingVelocityTest_);

	runUndulatorVelocityTest_ = new QPushButton("Run Undulator Velocity Tests");
	connect(runUndulatorVelocityTest_, SIGNAL(clicked(bool)), this, SLOT(onRunUndulatorVelocityTestClicked()));
	buttonsLayout->addWidget(runUndulatorVelocityTest_);

	buttonsLayout->addStretch();

	outputTextEdit_ = new QTextEdit();
	outputTextEdit_->setReadOnly(true);
	resultsLayout->addWidget(outputTextEdit_);


	resize(800, 600);
}


