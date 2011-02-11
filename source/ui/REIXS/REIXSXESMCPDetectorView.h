#ifndef REIXSXESMCPDETECTORVIEW_H
#define REIXSXESMCPDETECTORVIEW_H

#include <QWidget>
#include "ui/AMControlEditor.h"

#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotImage.h"

#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QProgressBar>

class REIXSXESMCPDetector;

class REIXSXESMCPDetectorView : public QWidget
{
	Q_OBJECT
public:
	REIXSXESMCPDetectorView(REIXSXESMCPDetector* detector, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// When the detector's countsPerSecond changes, we update the text indicator and progress bar.
	void onCountsPerSecondChanged(double countsPerSecond);
	/// When the user changes which image to view from the selector
	void onImageSelectorChanged(int index);

protected:
	/// The detector we provide a view for
	REIXSXESMCPDetector* detector_;

	/// UI components:
	MPlotWidget* imageView_;
	MPlot* imagePlot_;
	MPlotImageBasic* image_;


	QPushButton* clearButton_;
	QComboBox* imageSelector_;
	AMBasicControlEditor* averagingPeriodControl_;
	AMBasicControlEditor* persistDurationControl_;
	AMControlEditor* orientationControl_;

	QLabel* countsPerSecondIndicator_;
	QProgressBar* countsPerSecondBar_;


};

#endif // REIXSXESMCPDETECTORVIEW_H
