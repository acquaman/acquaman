#include "SGMTestsWindow.h"

#include "tests/SGM/TestSGMMonochromatorInfo.h"
SGMTestsWindow::SGMTestsWindow(QWidget *parent) : QMainWindow(parent)
{
    outputTextEdit_ = new QTextEdit();

    QGroupBox* mainGroupBox = new QGroupBox("Test Results");
    QHBoxLayout* mainGroupBoxLayout = new QHBoxLayout();
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

