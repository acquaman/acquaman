/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMACQSCANSPECTRUMOUTPUT_H
#define AMACQSCANSPECTRUMOUTPUT_H

#include "acquaman/dacq3_3/acqDataHandler.h"
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

#include "acquaman/dacq3_3/OutputHandler/acqTextSpectrumOutput.h"
#include <qdebug.h>
#include <qmap.h>
#include <QCoreApplication>
#include <string>


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

protected:
	virtual void onExpectationOfSpectrumNotMet();

private:
	int outputCol;
	AMScan *scan_;
	QObject *scanController_;
	QMap<int, double> dataDelayList_;
	QMap<int, QList<double> > spectraDelayList_;
	QMap<int, double> dataPackage_;
	QMap<int, QList<double> > spectraPackage_;
	QMap<int, double> extraPackage_;
	QHash<int, int> pvnoToColumn_;
	int colNo_, specColNo_;
	bool lockHash_;
	bool dataDelay_;
};

#endif /* __cplusplus */

#endif // AMACQSCANSPECTRUMOUTPUT_H
