#ifndef AMACQSCANOUTPUT_H
#define AMACQSCANOUTPUT_H

#include "acquaman/dacq3_2/acqDataHandler.h"
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

#include "acquaman/dacq3_2/OutputHandler/acqTextOutput.h"
#include <qdebug.h>
#include <qmap.h>
#include <string>

#include "../MPlot/src/MPlot/MPlotWidget.h"
#include "../MPlot/src/MPlot/MPlotSeriesData.h"
#include "../MPlot/src/MPlot/MPlotSeries.h"
#include "../MPlot/src/MPlot/MPlotImageData.h"
#include "../MPlot/src/MPlot/MPlotImage.h"
#include "../MPlot/src/MPlot/MPlotTools.h"


class AMAcqScanOutput : public acqTextOutput
{
public:
	// callbacks that are over-ridden from the acqTextOutput
	// by over-writing these functions, we can change what happens when they're called back.
	// these must be manually over-written when the class creator is called.
	static int startRecord( acqKey_t key, int eventno);
	static int endRecord( acqKey_t key, int eventno);
	static int putValue( acqKey_t key, int eventno, int pvno, const void *value, int count);
	static int shutdown( acqKey_t key);

	// standard creator and destructor.
	AMAcqScanOutput();
	virtual ~AMAcqScanOutput();
	// we need a static function to be called by the factory operator.
	static acqBaseOutput *new_AMAcqScanOutput();
	void setScan(AMScan *scan);

	// routines to output file header sections, over-riding the functions from the acqTextOutput Class
//	virtual int fileHeaderStart();
//	virtual int fileHeaderComment();
//	virtual int fileHeaderVerboseEvents();
//	virtual int fileHeaderTerseEvents();
//	virtual int fileHeaderDescribeEvents();
//	virtual int fileHeaderEnd();

	// overriding the setProperty function allows us to implement our own properties.
	virtual void setProperty(const std::string, const std::string);

protected:
	/// \todo Figure out why I had to go to public with that stuff, rather than just protected.
	class ScanPVPrivate : public pvPrivate{
	public:
		virtual int output(acqKey_t key, int dataType, const void *value, int count);
	};

	virtual void getPrivate(int eventID, int pvID);

private:
	int outputCol;
	AMScan *scan_;
	QMap<int, double> dataDelayList_;
	bool dataDelay_;

	MPlotWidget *plotWindow;
	MPlot *plot;
	MPlotSeriesBasic *series1;
	MPlotRealtimeModel *data1;
};

#endif /* __cplusplus */



#endif // AMACQSCANOUTPUT_H
