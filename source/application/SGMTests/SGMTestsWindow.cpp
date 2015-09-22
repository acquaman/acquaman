#include "SGMTestsWindow.h"

#include "tests/SGM/TestSGMMonochromatorInfo.h"
#include "tests/SGM/SGMMonochromatorInfoTestView.h"
#include "beamline/SGM/monochromator/SGMMonochromatorInfo.h"
SGMTestsWindow::SGMTestsWindow(QWidget *parent) : QMainWindow(parent)
{
    outputTextEdit_ = new QTextEdit();

    QGroupBox* mainGroupBox = new QGroupBox("Test Results");
    QHBoxLayout* mainGroupBoxLayout = new QHBoxLayout();
    showMonoViewButton_ = new QPushButton("Show Mono Test View");
    connect(showMonoViewButton_, SIGNAL(clicked(bool)), this, SLOT(onShowMonoViewClicked()));
    mainGroupBoxLayout->addWidget(showMonoViewButton_);
    mainGroupBoxLayout->addWidget(outputTextEdit_);

    mainGroupBox->setLayout(mainGroupBoxLayout);

    setCentralWidget(mainGroupBox);

    TestSGMMonochromatorInfo monoTests;
    monoTests.performTests();
    QStringList testResults = monoTests.testResults();
    QString resultString;
    foreach(QString result, testResults) {
        resultString.append(QString("\n%1").arg(result));
    }

    outputTextEdit_->setText(resultString);
    resize(640,480);
}

void SGMTestsWindow::onShowMonoViewClicked()
{
    SGMMonochromatorInfo* testMonoInfo = new SGMMonochromatorInfo(SGMGratingSupport::LowGrating,
                                                                  -412460.94,
                                                                  SGMUndulatorSupport::FirstHarmonic,
                                                                  13.2265442,
                                                                  0,
                                                                  358.199551,
                                                                  this);

    SGMMonochromatorInfoTestView* testView = new SGMMonochromatorInfoTestView(testMonoInfo);
    testView->show();
}

