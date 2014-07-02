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


#ifndef REIXSXESMCPDETECTORVIEW_H
#define REIXSXESMCPDETECTORVIEW_H

#include <QWidget>
#include "ui/beamline/AMControlEditor.h"

#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotImage.h"

#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QProgressBar>
#include <QToolButton>

class QDialog;

//class REIXSXESMCPDetectorPre2013;
class REIXSXESMCPDetector;

class REIXSXESMCPDetectorView : public QWidget
{
	Q_OBJECT
public:
 	virtual ~REIXSXESMCPDetectorView();
	//REIXSXESMCPDetectorView(REIXSXESMCPDetectorPre2013* detector, QWidget *parent = 0);
	REIXSXESMCPDetectorView(REIXSXESMCPDetector* detector, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// When the detector's countsPerSecond changes, we update the text indicator and progress bar.
	void onCountsPerSecondChanged(double countsPerSecond);
	/// When the user changes which image to view from the selector
	void onImageSelectorChanged(int index);

	/// When the button to adjust the plot image's color map is clicked
	void onAdjustColorMapButtonClicked();
	/// When the color map inside the dialog is changed
	void onColorMapChanged(const MPlotColorMap& map);

protected:
	/// The detector we provide a view for
	//REIXSXESMCPDetectorPre2013* detector_;
	REIXSXESMCPDetector* detector_;

	/// UI components:
	MPlotWidget* imageView_;
	MPlot* imagePlot_;
	MPlotImageBasic* image_;


	QPushButton* clearButton_;
	QComboBox* imageSelector_;
	AMControlEditor* averagingPeriodControl_;
	AMControlEditor* persistDurationControl_;
	// removed: AMControlEditor* orientationControl_;

	QLabel* countsPerSecondIndicator_;
	QProgressBar* countsPerSecondBar_;

	QToolButton* adjustColorMapButton_;
	QDialog* colorMapEditor_;

};

#endif // REIXSXESMCPDETECTORVIEW_H
