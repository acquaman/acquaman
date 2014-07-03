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


#include "AMProcessVariable.h"
#include "util/AMErrorMonitor.h"

// AMProcessVariable
/////////////////////////


// This a convenience function that we can call to figure out our simplified type based on an epics ca_field_type(). Possibilities are:
/*
#define DBF_STRING	0
#define	DBF_INT		1
#define	DBF_SHORT	1
#define	DBF_FLOAT	2
#define	DBF_ENUM	3
#define	DBF_CHAR	4
#define	DBF_LONG	5
#define	DBF_DOUBLE	6

//
//
//DBR_CHAR  	dbr_char_t  	8 bit character
//DBR_SHORT 	dbr_short_t 	16 bit integer
//DBR_ENUM 	dbr_enum_t 	16 bit unsigned integer
//DBR_LONG 	dbr_long_t 	32 bit signed integer
//DBR_FLOAT 	dbr_float_t 	32 bit IEEE floating point
//DBR_DOUBLE 	dbr_double_t 	64 bit IEEE floating point
//DBR_STRING 	dbr_string_t 	40 character string
//
//
  */
PVDataType::Type AMProcessVariable::serverType2ourType(chtype serverType) {

	switch(serverType) {
	case DBF_STRING:
		return PVDataType::String;
	case DBF_SHORT:
	case DBF_CHAR:
	case DBF_LONG:
		return PVDataType::Integer;
	case DBF_ENUM:
		return PVDataType::Enum;
	case DBF_FLOAT:
	case DBF_DOUBLE:
		return PVDataType::FloatingPoint;
	}
	return PVDataType::FloatingPoint;
}

chtype AMProcessVariable::serverType2StatusType(chtype serverType)
{
	switch(serverType) {
	case DBF_STRING:
		return DBR_STS_STRING;
	case DBF_SHORT:
	case DBF_CHAR:
	case DBF_LONG:
		return DBR_STS_LONG;
	case DBF_ENUM:
		return DBR_STS_ENUM;
	case DBF_FLOAT:
	case DBF_DOUBLE:
		return DBR_STS_DOUBLE;
	}
	return DBR_STS_DOUBLE;
}


AMProcessVariable::AMProcessVariable(const QString &pvName, bool monitor, QObject *parent, int connectionTimeoutMs) :
	QObject(parent)
{
	setObjectName("AMProcessVariable_" + pvName);
	shouldBeMonitoring_ = monitor;

	d_ = AMProcessVariableSupport::getPrivateForPVName(pvName);
    d_->attachProcessVariable(this);

	// This will notice if the connection times out:
	QTimer::singleShot(connectionTimeoutMs, this, SLOT(onConnectionTimeout()));
}

AMProcessVariable::~AMProcessVariable() {
	d_->detachProcessVariable(this);
}

void AMProcessVariable::onConnectionTimeout() {

	// If we haven't connected by now:
	if(!isConnected()) {
		// Reporting an error monitor here is expensive (especially if you have a lot of timeouts on startup) ... so we'll let the process variable support take care of it with a deferred call
		AMProcessVariableSupport::reportTimeoutError(pvName());
		emit connectionTimeout();
	}

	// Channel access will keep on trying in the background. (Channel access handles this automatically for us.)  Will emit connected() signal when successful.
}

int AMProcessVariable::binIntegerValues(int lowIndex, int highIndex) const{
	QVector<int> lastInts = lastIntegerValues();
	int lIndex = lowIndex;
	int hIndex = highIndex;
	int rVal = 0;
	if(hIndex < lIndex)
		return rVal;
	if(lIndex < 0)
		lIndex = 0;
	if(hIndex > lastInts.count())
		hIndex = lastInts.count();
	for(int x = lIndex; x < hIndex; x++)
		rVal += lastInts.at(x);
	return rVal;
}

double AMProcessVariable::binFloatingPointValues(int lowIndex, int highIndex) const{
	QVector<double> lastDoubles = lastFloatingPointValues();
	int lIndex = lowIndex;
	int hIndex = highIndex;
	double rVal = 0.0;
	if(hIndex < lIndex)
		return rVal;
	if(lIndex < 0)
		lIndex = 0;
	if(hIndex > lastDoubles.count())
		hIndex = lastDoubles.count();
	for(int x = lIndex; x < hIndex; x++)
		rVal += lastDoubles.at(x);
	return rVal;
}


