#include "SGMTestsWindow.h"

#include "tests/SGM/SGMEnergyPositionTest.h"
#include "tests/SGM/SGMEnergyPositionTestView.h"
#include "tests/SGM/SGMEnergyTrajectoryTestView.h"
#include "tests/SGM/SGMEnergyControlTestView.h"
#include "qjson/serializer.h"

#include "beamline/SGM/monochromator/SGMEnergyPosition.h"
#include <QDebug>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

SGMTestsWindow::SGMTestsWindow(QWidget *parent) : QMainWindow(parent)
{
    energyTestView_ = 0;
    energyTrajectoryTestView_ = 0;
    energyControlTestView_ = 0;
    setupUi();

    SGMEnergyPositionTest monoTests;
    monoTests.performTests();    

    QStringList testResults = monoTests.testResults();

    QString resultString;
    foreach(QString result, testResults) {
        resultString.append(QString("\n%1").arg(result));
    }

    outputTextEdit_->setText(resultString);
    produceTestUndulatorValues();
}

void SGMTestsWindow::onShowEnergyViewClicked()
{
    if(energyTestView_ == 0) {

        SGMEnergyPosition* testEnergyPosition = new SGMEnergyPosition(SGMGratingSupport::LowGrating,
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

void SGMTestsWindow::produceTestUndulatorValues()
{
	QString allValues;

	allValues.append("Time\tUndulatorPosition\n");

	SGMEnergyPosition testEnergyPosition(1200, SGMGratingSupport::HighGrating);

	int time = 1;

	while(testEnergyPosition.resultantEnergy() < 2000) {

		allValues.append(QString("%1\t\t%2\n").arg(time).arg(testEnergyPosition.undulatorPosition()));

		testEnergyPosition.requestEnergy(testEnergyPosition.resultantEnergy() + 10);
		++time;
	}

	qDebug() << allValues;

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

    buttonsLayout->addStretch();

    outputTextEdit_ = new QTextEdit();
    outputTextEdit_->setReadOnly(true);
    resultsLayout->addWidget(outputTextEdit_);


    resize(800, 600);
}

