
#include "acqTextSpectrumOutput.h"

#include "acqFileStream.h"

#include "acqFactory.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdarg.h>
#include <string.h>

#define DEBUG(PTR) if((PTR)->getDebug())

using namespace std;
#include <sstream>

#define TRUE 1
#define FALSE 0

static acqOutputHandlerFactoryRegister registerMe( "Text", acqTextSpectrumOutput::new_acqTextSpectrumOutput);

/// Only constructor. This over-rides the default handler entries from a base class.
acqTextSpectrumOutput::acqTextSpectrumOutput() :
	spectrumPropertyPrefix("Spectrum:") ,
	needSpectrumDelimiter(0), spectrumDelimiter(","),
	haveSpectrum(0), spectrumFormat(SF_DEFAULT),
	spectrumStream(NULL),
	ts_outputState(TSOH_INIT),
	spectrumSplit(SS_ONE_TO_ONE)
{
	handler.start_cb = start;
	handler.pause_cb = pause;
	handler.resume_cb = resume;
	handler.stop_cb = stop;
	handler.shutdown_cb = shutdown;
	handler.startRecord_cb = startRecord;
	handler.endRecord_cb = endRecord;
	handler.nextOutput_cb = nextOutput;
	handler.pvFlags_cb = pvFlags;
	defProperty(TEXTS_SPECTRUM_SPLIT, "OneToOne,ByRecord,ByInstance");

	setPropertyList( TEXTS_NAME_SPECTRUM "," TEXTS_PROP_SPECTRUM "=Text," TEXTS_SPECTRUM_SPLIT "=OneToOne," TEXTS_SPECTRUM_STREAM "=File");
	DEBUG(this) printf("End acqTextSpectrumOutput::constructor()\n");
}

acqTextSpectrumOutput::~acqTextSpectrumOutput()
{
}

acqBaseOutput *
acqTextSpectrumOutput::new_acqTextSpectrumOutput()
{
	return new acqTextSpectrumOutput();
}

/// C interface to the Constructor.
/// these are important for dynamic library use!
acqKey_t new_acqTextSpectrumOutput(void)
{
	acqBaseOutput *to = new acqTextSpectrumOutput();
	return (acqKey_t )to;
}

/// start of a run - may be multiple passes
/// convert the flag strings into instances in private structures, and update the private entries
/// for all the text fields.
int acqTextSpectrumOutput::start( acqKey_t key)
{
	acqTextSpectrumOutput *to = (acqTextSpectrumOutput *)key;

	DEBUG(to) printf("acqTextSpectrumOutput::start(%p)\n", key);
	to->haveSpectrum = FALSE;
	acqTextOutput::start(key);
	return 0;
}

// over-ride of the virtual class for getting a 'private' pointer for per-PV information
void acqTextSpectrumOutput::getPrivate(int eventID, int pvID)
{
	int uid = makeuid(eventID, pvID);
	if( pvInfo.find(uid) == pvInfo.end() )
		pvInfo.insert( pair<int, pvPrivate*>(uid, new pvTSPrivate) );
}

// additional checking for pvFlags:
int acqTextSpectrumOutput::pvFlags( acqKey_t key, int eventno, int pvno, const char *eflags)
{
	// let the ancestors update their tables
	acqTextOutput::pvFlags( key, eventno, pvno, eflags);

	acqTextSpectrumOutput *to = (acqTextSpectrumOutput *)key;

	int uid = makeuid(eventno,pvno);

	pvInfo_iter pvIter;
	pvIter = to->pvInfo.find(uid);
	if( pvIter == to->pvInfo.end() )
		return -1;
	pvTSPrivate *pvp;
	pvp = dynamic_cast<pvTSPrivate *>((*pvIter).second);
	if( pvp == NULL)
		return -1;

	std::string isSpectrumKey("isSpectrum");

	pvPrivate::pvproperties_iter checkSpectrum;
	checkSpectrum = pvIter->second->pvproperties.find( isSpectrumKey );

	if( checkSpectrum != pvIter->second->pvproperties.end() )
	{
		DEBUG(to) printf("pv[%d] isSpectrum:%s\n", uid, checkSpectrum->second.c_str() );

		if(checkSpectrum->second == "1" || checkSpectrum->second == "TRUE" || checkSpectrum->second == "true")
		{
			pvp->isSpectrum = true;
			to->haveSpectrum = TRUE;
		}
		return 0;
	}

	pvp->isSpectrum = false;
	return 0;
}

// system is shutting down. perform any necessary cleanup
int acqTextSpectrumOutput::shutdown( acqKey_t key)
{
	acqTextOutput::shutdown(key);

	acqTextSpectrumOutput *to = (acqTextSpectrumOutput *)key;

	DEBUG(to) printf("acqTextSpectrum::shutdown(%p)\n", key);
	if( to->spectrumStream)
		to->spectrumStream->close();
	return 0;
}

///
// output the file header prefix.
///
int acqTextSpectrumOutput::fileHeaderStart()
{
	//
	// output standard spectrum headers
	//

	acqTextOutput::fileHeaderStart();
	// append information
	if( haveSpectrum && spectrumStream)
	{
		string name = spectrumStream->getProperty("Output File");
		if( ! name.empty() )
			sendOutputLine( "# Spectra file: %s\n", name.c_str() );
	}
	return 1;
}

int acqTextSpectrumOutput::spectrumFileHeaderStart()
{

	// Spectrum header
	if( spectrumFormat == SF_BINARY )
	{
		long magic = 0x01020304;
		unsigned char *magicp = (unsigned char *) &magic;

		sendSpectrumLine( "#binary byte-order:%02d%02d%02d%02d\n",magicp[0], magicp[1], magicp[2], magicp[3]);
	}
	else // SF_DEFAULT, SF_TEXT
	{
		sendSpectrumLine( "#text\n");
	}
	return 0;
}


//
// Generate the next output file
// generate all output file names.
//
int acqTextSpectrumOutput::nextOutput( acqKey_t key)
{

	acqTextSpectrumOutput *to = (acqTextSpectrumOutput *)key;

	DEBUG(to) printf("acqTextSpectrumOutput::nextOutput\n");
	if( to->outputState == TOH_EMPTY)
			return 0;		// OK, redundant call

	acqTextOutput::nextOutput(key);		// set up the next text file

	// this depends on PV information having been transmitted before acquisition
	// looping starts. If no PV's have been detected that have been flagged 'spectra',
	// then no file will be created.
	if( to->haveSpectrum && to->spectrumStream)
	{
		to->recordCount = 0;
		if( to->spectrumSplit == SS_ONE_TO_ONE)
		{
			to->nextSpectrumFile();
		}

	}

	return 0;
}

// put out flagged header entries: event ID, timestamps, comment prefix ...
int acqTextSpectrumOutput::startRecord( acqKey_t key, int eventno)
{
	acqTextSpectrumOutput *to = (acqTextSpectrumOutput *)key;

	DEBUG(to) printf("startRecord(%p, %d)\n", key, eventno);

	// flag that some output is occuring
	acqTextOutput::startRecord(key, eventno);

	to->recordCount++;
	if( to->spectrumSplit == SS_BY_RECORD)
		to->nextSpectrumFile(to->recordCount);

	return 0;
}

int acqTextSpectrumOutput::endRecord( acqKey_t key, int eventno)
{
	acqTextSpectrumOutput *to = (acqTextSpectrumOutput *)key;

	acqTextOutput::endRecord( key, eventno);

	if( to->spectrumStream && to->needSpectrumDelimiter )	// only true if there is a spectrum file in text format
	{
		to->sendSpectrumLine( "\n");
		to->spectrumStream->flush();
		to->needSpectrumDelimiter = 0;
	}

	return 0;
}
string
acqTextSpectrumOutput::default_stream()
{
	return "File";
}

std::string acqTextSpectrumOutput::nextSpectrumFile(int record, int spectrum_no)
{
	outputFullPath = getProperty(PROP_OUTPUT_FILE);

	DEBUG(this) printf("nextSpectrumFile(%d,%d) from %s\n", record, spectrum_no, outputFullPath.c_str()) ;

	int slash = outputFullPath.rfind("/");

	std::string directory = outputFullPath.substr(0, slash);
	std::string newfile;
	std::string dat;
	if( slash >= 0)
		newfile = outputFullPath.substr(slash+1);
	else
		newfile = outputFullPath;

	int dot = newfile.rfind(".");
	if( dot > 0 && newfile.length() - dot <= 5)
	{
		dat = newfile.substr(dot, newfile.length() );
		newfile.replace(dot, dat.length(), "");
		newfile = newfile.substr(0,dot);
	}

	switch (spectrumSplit)
	{
	case SS_ONE_TO_ONE:
		// conventional naming: take the generated name, and make it *_spectra.SUFFIX
		spectrumFileName = newfile + "_spectra" + dat;
		break;
	case SS_BY_RECORD:
		// add _spectra_RECORDID.SUFFIX
		if( record == -1)
			spectrumFileName = newfile + "_spectra_%05d" + dat;
		else
			spectrumFileName = newfile + "_spectra_" + to_string<int>(record,5) + dat;
		break;
	case SS_BY_INSTANCE:
		// add _spectra_RECORDID.PVID.SUFFIX
		if( record == -1 || spectrum_no == -1)
			spectrumFileName = newfile + "_spectra_%05d_%03d" + dat;
		else
			spectrumFileName = newfile + "_spectra_" + to_string<int>(record,5) + "_" + to_string<int>(spectrum_no,3) + dat;
		break;
	}
	if( spectrumStream)
	{
		spectrumStream->setProperty( PROP_FILE_PATH, directory);
		spectrumStream->setProperty( PROP_FILE_TEMPLATE, spectrumFileName);
		spectrumStream->next();
	}
	return spectrumFileName;
}

// virtual function for writing PV's
int acqTextSpectrumOutput::pvTSPrivate::output( acqKey_t key, int dataType, const void *value, int count)
{
	acqTextSpectrumOutput *to = (acqTextSpectrumOutput *)key;
	char result[100];
	unsigned long recordOffset;	// file offset

	// if not a spectrum value, output as a 'regular' text entry
	if( isSpectrum == false)
		return pvPrivate::output(key, dataType, value, count);

	DEBUG(to) printf("output( %p, %d, %p, %d)\n", key, dataType, value, count);

	// Spectrum value.
	if( to->spectrumStream == NULL || ! to->spectrumStream->isReady() )	// everything ... breaks?
	{
		int val = -1;

		pvPrivate::output(key, DBF_LONG, &val, 1);
		return -1;
	}

	if( value == NULL || colp->dataSize == 0 || colp->dataCount == 0)
	{
		pvPrivate::output(key, DBF_LONG, NULL, 1);	// records the "no Data" flag
		return -1;
	}

	if( to->spectrumSplit == SS_BY_INSTANCE)
		to->nextSpectrumFile(to->recordCount, colp->columnID);

	// write the spectrum to the file
	switch( to->spectrumFormat)
	{
	default:
		break;
	case SF_BINARY:

		recordOffset = to->spectrumStream->offset();

		to->sendSpectrumData(value, colp->dataSize* colp->dataCount);
		break;

	case SF_DEFAULT:
	case SF_TEXT:
		// special adjustment to get the offset to be after the delimiter
		if( to->needSpectrumDelimiter)
		{
			to->sendSpectrumLine( "%s", to->getSpectrumDelimiter().c_str() );
			to->needSpectrumDelimiter = 0;
		}
		recordOffset = to->spectrumStream->offset();

		for( int i=0; i < count; i++)
		{
			if( to->needSpectrumDelimiter)
				to->sendSpectrumLine( "%s", to->getSpectrumDelimiter().c_str() );

			to->needSpectrumDelimiter = TRUE;
			if (to->value_to_string(value, colp->columnType, format, result, sizeof result) < 0)
			{
				to->sendSpectrumLine( "<No Data>");
				continue;
			}
			to->sendSpectrumLine( "%s", result);
			/* NTBA May 8th, 2011 David Chevrier
			   There seems to be an issue with the DBF_LONG being saved as int (size 4)
			   rather than as long (size 8). Probably a 64bit problem.
			 */
			if(colp->columnType == DBF_LONG)
				value = (char  *)value + 4;
			else
				value = (char  *)value + colp->dataSize;
		}

		break;
	}
	switch( to->spectrumSplit)
	{
	default:
	case SS_ONE_TO_ONE:
		acqTextOutput::pvPrivate::output(key, DBF_LONG, &recordOffset, 1);
		break;

	case SS_BY_RECORD:
		snprintf( result, sizeof result, "<%s %ld>", to->spectrumFileName.c_str(), recordOffset );
		acqTextOutput::pvPrivate::output(key, DBF_STRING, result, 1);
		break;

	case SS_BY_INSTANCE:
		snprintf( result, sizeof result, "<%s>", to->spectrumFileName.c_str());
		acqTextOutput::pvPrivate::output(key, DBF_STRING, result, 1);
		break;
	}

	return 0;

}

int
acqTextSpectrumOutput::sendSpectrumLine( const char *format, ...)
{
	va_list argPtr;
	int err;
	char buffer[512];

	if( format == NULL || spectrumStream == NULL || ! spectrumStream->isReady() )
	{
		handlerSignal( acqTextOutput_WriteError);
		return -1;
	}

	va_start( argPtr, format);

	vsprintf(buffer, format, argPtr);

	err = spectrumStream->write( buffer, strlen(buffer) );
	if( err < 0)
		handlerSignal( acqTextOutput_WriteError);

	va_end(argPtr);
	return err;

}

int
acqTextSpectrumOutput::sendSpectrumData( const void *data, unsigned int dataSize)
{
	int err;

	if( data == NULL || spectrumStream == NULL || ! spectrumStream->isReady() )
	{
		handlerSignal( acqTextOutput_WriteError);
		return -1;
	}
	err = spectrumStream->write( data, dataSize);
	if (err <= 0)
		handlerSignal( acqTextOutput_WriteError);

	return err;
}

//
///
// Property manipulation.
// If a property is not in this list, it is still assigned in the base property list.
// It is recommended that any function derived from acqTextOutput should
// override setProperty if it has its own property list, and explicitly call
// acqTextOutput::setProperty for inherited properties.
///
void acqTextSpectrumOutput::setProperty(const string name, const string value)
{

	DEBUG(this) printf("Spectrum::SetProperty(%s,%s)\n", name.c_str(), value.c_str() );

	if( name == TEXTS_PROP_SPECTRUM)
	{
		if( value == "text" || value == "Text")
			spectrumFormat = SF_TEXT;
		else if( value == "binary" || value == "Binary")
			spectrumFormat = SF_BINARY;
		else
			spectrumFormat = SF_DEFAULT;

	}
	else if( name == TEXTS_NAME_SPECTRUM)
	{
		// set the spectrum file name: is this really required?
	}
	else if( name == TEXTS_SPECTRUM_SPLIT)
	{
		// set the spectrum type of splitting
		if( value == "OneToOne")
			spectrumSplit = SS_ONE_TO_ONE;
		else if( value == "ByRecord")
			spectrumSplit = SS_BY_RECORD;
		else if( value == "ByInstance")
			spectrumSplit = SS_BY_INSTANCE;
	}
	else if( name == TEXTS_SPECTRUM_STREAM)
	{
		// change the stream type.
		if( spectrumStream == NULL || spectrumStream->streamName() != value)
		{
			if( spectrumStream)
				delete spectrumStream;
			spectrumStream = acqOutputStreamFactoryRegister::new_acqStream(value);
		}
	}
	else if( spectrumStream && name.find(spectrumPropertyPrefix) == 0)
	{
		// property that is part of the spectrum output stream.
		spectrumStream->setProperty( name.substr(spectrumPropertyPrefix.length() ), value);
	}
	else if(name == PROP_OUTPUT_FILE)
	{
		outputFullPath = value;
	}

	acqTextOutput::setProperty(name, value);
	DEBUG(this) printf("End setProperty\n");
}

// over-ride valid property test in order to handle prefixed property names
bool acqTextSpectrumOutput::validProperty(const string name)
{
	DEBUG(this) printf("validProperty(%s)\n", name.c_str() );

	if( acqBaseOutput::validProperty(name) )
		return true;
	if( spectrumStream)
		return spectrumStream->validProperty( name.substr(spectrumPropertyPrefix.length() ));
	return false;
}

const std::string acqTextSpectrumOutput::getPropertyList()
{
	DEBUG(this) printf("acqTextSpectrumOutput::getPropertyList()\n");

	string names = acqBaseOutput::getPropertyList();
	if( spectrumStream)
	{
		std::vector<std::string> proplist = takeApart(spectrumStream->getPropertyList(), ",");

		for( unsigned int idx=0; idx < proplist.size() ; idx++)
			names = names + "," + spectrumPropertyPrefix + proplist[idx];
	}
	return names;
}

std::string &acqTextSpectrumOutput::getProperty(std::string name)
{
	DEBUG(this) printf("acqTextSpectrumOutput::getProperty(%s)\n", name.c_str() );

	if( spectrumStream && name.find(spectrumPropertyPrefix) == 0)
		return spectrumStream->getProperty(name.substr(spectrumPropertyPrefix.length()));

	return acqTextOutput::getProperty(name);
}

