#ifndef AMACQSCANOUTPUT_H
#define AMACQSCANOUTPUT_H

#include "acqDataHandler.h"

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

	// routines to output file header sections, over-riding the functions from the acqTextOutput Class
	virtual int fileHeaderStart();
	virtual int fileHeaderComment();
	virtual int fileHeaderVerboseEvents();
	virtual int fileHeaderTerseEvents();
	virtual int fileHeaderDescribeEvents();
	virtual int fileHeaderEnd();

	// overriding the setProperty function allows us to implement our own properties.
	virtual void setProperty(const std::string, const std::string);

protected:
	/// \todo Figure out why I had to go to public with that stuff, rather than just protected.
	class ScanPVPrivate : public pvPrivate{
	public:
		virtual int output(acqKey_t key, int dataType, const void *value, int count){
			acqTextOutput *to = (acqTextOutput *)key;
			char result[100];

			for(int i=0; i < count; i++)
			{
				if( to->needDelimiter)
					to->sendOutputLine( "%s", to->delimiter.c_str() );

				if( value == NULL)
				{
					to->sendOutputLine( "<No Connection>");
					to->needDelimiter = true;
					continue;
				}
				qDebug() << "Trying to switch on type";
				switch( dataType)
				{
				case DBF_STRING:
					qDebug() << "It's a string: " << (char *)value;
//					strncpy( result, (char *)value, result_max);
					break;
				case DBF_ENUM:	// ENUM support is difficult if the value may be passed through multiple
						// processes before being handled. It is the responsibility of the inheriting class
						// to determine how to convert enums to strings, probably with support from the
						// calling program.
				case DBF_SHORT:
					qDebug() << "It's a short: " << *(short *)value;
//					snprintf(result, result_max, format?format:"%d", *(short *)value);
					break;
				case DBF_FLOAT:
					qDebug() << "It's a float: " << *(float *)value;
//					snprintf(result, result_max, format?format:"%g", *(float *)value);
					break;
				case DBF_CHAR:
					qDebug() << "It's a char: " << *(char *)value;
//					snprintf(result, result_max, format?format:"%d", *(char *)value);
					break;
				case DBF_LONG:
					qDebug() << "It's a long: " << *(long *)value;
//					snprintf(result, result_max, format?format:"%ld", *(long *)value);
					break;
				case DBF_DOUBLE:
					qDebug() << "It's a double: " << *(double *)value;
//					snprintf(result, result_max, format?format:"%g", *(double *)value);
					break;
				default:
//					handlerSignal(acqBaseOutput_BadType);
					return -1;
				}

				if (to->value_to_string(value, dataType, format, result, sizeof result) < 0)
				{
					to->sendOutputLine( "<No Data>");
					to->needDelimiter = true;
					continue;
				}
				to->sendOutputLine( "%s", result);
				to->needDelimiter = true;

			}
			return 0;
		}
	};

	virtual void getPrivate(int eventID, int pvID){	// map info allocator - derived classes should over-ride
		int uid = makeuid(eventID, pvID);

		if( pvInfo[uid] == NULL)
			pvInfo[uid] = new ScanPVPrivate;
	}

private:
	int outputCol;
};

#endif /* __cplusplus */



#endif // AMACQSCANOUTPUT_H
