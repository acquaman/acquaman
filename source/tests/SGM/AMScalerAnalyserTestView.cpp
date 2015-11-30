#include "AMScalerAnalyserTestView.h"

#include <QLabel>
#include <QVBoxLayout>

#include "util/AMScalerCountAnalyser.h"
AMScalerAnalyserTestView::AMScalerAnalyserTestView(QWidget *parent) :
	QWidget(parent)
{
	setupUi();
}

void AMScalerAnalyserTestView::onRunAnalysisButtonClicked()
{
	// Parse the input data
	QVector<double> monoCounts;

	QStringList inputData = scalerDataTextEdit_->toPlainText().split(",");

	bool parseSuccess = true;

	for (int iInput = 0, inputCount = inputData.count();
	     iInput < inputCount && parseSuccess;
	     ++iInput) {

		double currentValue = inputData.at(iInput).toInt(&parseSuccess);

		if(parseSuccess) {
			monoCounts.append(currentValue);
		}

	}



	if(parseSuccess) {
		// Smooth the mono data across 10 elements
		QVector<double> smoothedMonoCounts = QVector<double>(monoCounts.size(), 0);
		for (int iMonoCount = 5, lastMonoCount = monoCounts.count() - 5;
		     iMonoCount < lastMonoCount;
		     ++iMonoCount) {

			smoothedMonoCounts[iMonoCount] = qRound((monoCounts[iMonoCount - 5] +
								 monoCounts[iMonoCount - 4] +
								 monoCounts[iMonoCount - 3] +
								 monoCounts[iMonoCount - 2] +
								 monoCounts[iMonoCount - 1] +
								 monoCounts[iMonoCount] +
								 monoCounts[iMonoCount + 1] +
								 monoCounts[iMonoCount + 2] +
								 monoCounts[iMonoCount + 3] +
								 monoCounts[iMonoCount + 4] +
								 monoCounts[iMonoCount + 5]) / 10.0);

		}

		AMScalerCountAnalyser testAnalyser = AMScalerCountAnalyser(smoothedMonoCounts, 0, 15);
		resultsTextEdit_->setText(testAnalyser.toString());
	} else {

		resultsTextEdit_->setText("Failed to parse input");
	}
}

void AMScalerAnalyserTestView::setupUi()
{
	QVBoxLayout* mainLayout = new QVBoxLayout();

	QLabel* instructionsLabel = new QLabel("Paste test data in comma separated form below:");
	mainLayout->addWidget(instructionsLabel);

	scalerDataTextEdit_ = new QTextEdit();
	mainLayout->addWidget(scalerDataTextEdit_);

	runDataAnalysis_ = new QPushButton("Run Analysis");
	mainLayout->addWidget(runDataAnalysis_);
	connect(runDataAnalysis_, SIGNAL(clicked()), this, SLOT(onRunAnalysisButtonClicked()));

	resultsTextEdit_ = new QTextEdit();
	mainLayout->addWidget(resultsTextEdit_);

	setLayout(mainLayout);
}
