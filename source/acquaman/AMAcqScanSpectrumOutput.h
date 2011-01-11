#ifndef AMACQSCANSPECTRUMOUTPUT_H
#define AMACQSCANSPECTRUMOUTPUT_H

#include "acquaman/dacq3_2/acqDataHandler.h"
//#include "acqDataHandler.h"
//#include "dataman/AMXASScan.h"
#include "dataman/AMScan.h"
#include "ui/AMScanView.h"

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
#include <QCoreApplication>
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
	static int endRecord( acqKey_t key, int eventno);
	static int putValue( acqKey_t key, int eventno, int pvno, const void *value, int count);

	AMAcqScanSpectrumOutput();
	virtual ~AMAcqScanSpectrumOutput();
	static acqBaseOutput *new_AMAcqScanSpectrumOutput();
	void setScan(AMScan *scan);
	void setScanController(QObject *scanController);

private:
	int outputCol;
	AMScan *scan_;
	QObject *scanController_;
	QMap<int, double> dataDelayList_;
	QMap<int, QList<double> > spectraDelayList_;
	QMap<int, double> dataPackage_;
	QMap<int, QList<double> > spectraPackage_;
	QHash<int, int> pvnoToColumn_;
	int colNo_, specColNo_;
	bool lockHash_;
	bool dataDelay_;

	/*MPlotWidget *plotWindow;
	MPlot *plot;
	MPlotSeriesBasic *series1, *series2, *series3;
	MPlotRealtimeModel *data1;

	AMScanView *sv;*/
};

#endif /* __cplusplus */

#endif // AMACQSCANSPECTRUMOUTPUT_H
