#ifndef AMACQSCANSPECTRUMOUTPUT_H
#define AMACQSCANSPECTRUMOUTPUT_H

#include "acquaman/dacq3_2/acqDataHandler.h"
//#include "acqDataHandler.h"
//#include "dataman/AMXASScan.h"
#include "dataman/AMScan.h"

#if defined(__cplusplus)
extern "C" {
#endif
/*
 * this function is C-callable, allowing a C program to make use of the handlers
 */
acqKey_t new_AMAcqScanOutput(void);
#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)

#include "acquaman/dacq3_2/OutputHandler/acqTextSpectrumOutput.h"
#include <qdebug.h>
#include <qmap.h>
#include <string>

#include "../MPlot/src/MPlot/MPlotWidget.h"
#include "../MPlot/src/MPlot/MPlotSeriesData.h"
#include "../MPlot/src/MPlot/MPlotSeries.h"
#include "../MPlot/src/MPlot/MPlotImageData.h"
#include "../MPlot/src/MPlot/MPlotImage.h"
#include "../MPlot/src/MPlot/MPlotTools.h"

class AMAcqScanSpectrumOutput : public acqTextSpectrumOutput
{
public:
	// callbacks that are over-ridden from the acqTextOutput
	// by over-writing these functions, we can change what happens when they're called back.
	// these must be manually over-written when the class creator is called.
	static int startRecord( acqKey_t key, int eventno);
//	static int endRecord( acqKey_t key, int eventno);
	static int putValue( acqKey_t key, int eventno, int pvno, const void *value, int count);

	AMAcqScanSpectrumOutput();
	virtual ~AMAcqScanSpectrumOutput();
	static acqBaseOutput *new_AMAcqScanSpectrumOutput();
	void setScan(AMScan *scan){
		qDebug() << "In spectrumOutput set scan";
		scan_ = scan;
	//	series1->setModel(data1);
		series1->setModel(scan_->channel(2));
		plot->addItem(series1);
		plot->setScalePadding(5);
		plot->enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);
		plotWindow->resize(450, 450);
		plotWindow->show();
	}

private:
	int outputCol;
	AMScan *scan_;
	QMap<int, double> dataDelayList_;
	QMap<int, QList<double> > spectraDelayList_;
	bool dataDelay_;

	MPlotWidget *plotWindow;
	MPlot *plot;
	MPlotSeriesBasic *series1;
	MPlotRealtimeModel *data1;
};

#endif /* __cplusplus */

#endif // AMACQSCANSPECTRUMOUTPUT_H
