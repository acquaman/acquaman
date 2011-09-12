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
};

#endif /* __cplusplus */



#endif // AMACQSCANOUTPUT_H
