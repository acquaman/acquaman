#ifndef AMSCALERANALYSERTESTVIEW_H
#define AMSCALERANALYSERTESTVIEW_H

#include <QWidget>

#include <QTextEdit>
#include <QPushButton>

/*!
  * A view class which acts as a form for testing the analysis of scaler count data.
  */
class AMScalerAnalyserTestView : public QWidget
{
    Q_OBJECT
public:
    explicit AMScalerAnalyserTestView(QWidget *parent = 0);

signals:

public slots:
protected slots:
	/// Runs the analysis on the input data, if it can be parsed
	void onRunAnalysisButtonClicked();
protected:

	/// Constructs and lays out the ui widgets
	void setupUi();

	QTextEdit* scalerDataTextEdit_;
	QPushButton* runDataAnalysis_;
	QTextEdit* resultsTextEdit_;
};

#endif // AMSCALERANALYSERTESTVIEW_H
