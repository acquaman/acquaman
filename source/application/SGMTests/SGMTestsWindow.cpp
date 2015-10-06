#include "SGMTestsWindow.h"

#include "tests/SGM/SGMEnergyPositionTest.h"
#include "tests/SGM/SGMEnergyPositionTestView.h"
#include "tests/SGM/SGMEnergyTrajectoryTestView.h"


#include "beamline/SGM/monochromator/SGMEnergyPosition.h"
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

SGMTestsWindow::SGMTestsWindow(QWidget *parent) : QMainWindow(parent)
{
    monoInfoTestView_ = 0;
    energyTestView_ = 0;
    setupUi();

    SGMEnergyPositionTest monoTests;
    monoTests.performTests();    

    QStringList testResults = monoTests.testResults();

    QString resultString;
    foreach(QString result, testResults) {
        resultString.append(QString("\n%1").arg(result));
    }

    outputTextEdit_->setText(resultString);
}

void SGMTestsWindow::onShowMonoViewClicked()
{
    if(monoInfoTestView_ == 0) {

        SGMEnergyPosition* testEnergyPosition = new SGMEnergyPosition(SGMGratingSupport::LowGrating,
                                                                      -412460.94,
                                                                      SGMUndulatorSupport::FirstHarmonic,
                                                                      13.2265442,
                                                                      0,
                                                                      358.199551,
                                                                      this);

        monoInfoTestView_ = new SGMEnergyPositionTestView(testEnergyPosition);
    }

    monoInfoTestView_->show();
}

void SGMTestsWindow::onShowTrajectoryViewClicked()
{

    if(energyTestView_ == 0) {

        energyTestView_ = new SGMEnergyTrajectoryTestView();
        energyTestView_->resize(1024,768);
        energyTestView_->show();
    }
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

    showMonoViewButton_ = new QPushButton("Show Mono Test View");
    connect(showMonoViewButton_, SIGNAL(clicked(bool)), this, SLOT(onShowMonoViewClicked()));
    buttonsLayout->addWidget(showMonoViewButton_);

    showTrajectoryViewButton_ = new QPushButton("Show Trajectory Test View");
    connect(showTrajectoryViewButton_, SIGNAL(clicked(bool)), this, SLOT(onShowTrajectoryViewClicked()));
    buttonsLayout->addWidget(showTrajectoryViewButton_);
    buttonsLayout->addStretch();

    outputTextEdit_ = new QTextEdit();
    outputTextEdit_->setReadOnly(true);
    resultsLayout->addWidget(outputTextEdit_);


    resize(800, 600);
}

