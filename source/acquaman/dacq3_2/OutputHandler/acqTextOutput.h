/*
 * Copyright 2007 Canadian Light Source, Inc. All Rights Reserved.
 */
/*
 * default Text Output data stream
 *
 */

//#if !defined(ACQTEXTOUTPUT_H)
#ifndef DACQLIB_ACQTEXTOUTPUT_H
#define DACQLIB_ACQTEXTOUTPUT_H 1
//#include "acqDataHandler.h"
#include "../acqDataHandler.h"

#if defined(__cplusplus)
extern "C" {
#endif
#define acqTextOutputID 1
#define acqTextOutput_CannotOpen acqOHsignal( 0, acqTextOutputID, 1)
#define acqTextOutput_WriteError acqOHsignal( 0, acqTextOutputID, 2)
#define acqTextOutput_NewOutput acqOHsignal( 1, acqTextOutputID, 3)

void acqTextOutput_setComment(void *key, char *comment);
void acqTextOutput_setVerbose(void*key, int seqno);
acqKey_t new_acqTextOutput(void);
#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)

#include "acqBaseOutput.h"
#include <string>
#include <vector>
#include <map>
#include <sys/time.h>

using namespace std;

// The state of the data output file: if there is no open file, then
// the state is TOH_INIT. If there is an open file, then TOH_EMPTY
// means the file has been created, but nothing has been written.
// TOH_HAS_CONTENT means that output to the file has occured.
//
enum toh_outputState { TOH_INIT, TOH_EMPTY, TOH_HAS_CONTENT };


class acqTextOutput : public acqBaseOutput {
public:
	enum spectrumFileEnum { SF_DEFAULT, SF_BINARY, SF_TEXT };
//private:
public:
	string comment;			// this gets inserted into the file header
	string delimiter;		// output delimiter in text files
	int needDelimiter;		// true when the next output should be preceded by a delimiter
	int curPassNo;			// track the pass numbers
public:
	acqTextOutput();
	virtual ~acqTextOutput();
	static acqBaseOutput * new_acqTextOutput();
	void setComment(string s) {comment = s;};
	void setVerbose(bool flag) { verbose = flag; } ;

	string const &getComment() { return comment; };
	string const &getDelimiter() { return delimiter; } ;

	// using the file path and template, generate the
	// next sequence number to be used.
	bool getVerbose() {return verbose; };

	// callbacks from data acquisition to output handler
	static int startRepeat( acqKey_t key, int passno);
	// static int outfile( acqKey_t key, char *pathname, char *nameTemplate);
	static int start( acqKey_t key);
	static int pause( acqKey_t key);
	static int resume( acqKey_t key);
	static int stop( acqKey_t key);
	static int shutdown( acqKey_t key);
	static int startRecord( acqKey_t key, int eventno);
	static int endRecord( acqKey_t key, int eventno);
	static int putValue( acqKey_t key, int eventno, int pvno, const void *value, int count);
	static int nextOutput(acqKey_t key);
	static int userComment( acqKey_t key, const char * commentString);
	static int eventFlags(acqKey_t key, int eventno, const char *flags);
	static int pvFlags( acqKey_t key, int eventno, int pvno, const char *flags);

	int sendOutputLine(const char * format, ...);
	// routines to output file header sections
	virtual int fileHeaderStart();
	virtual int fileHeaderComment();
	virtual int fileHeaderVerboseEvents();
	virtual int fileHeaderTerseEvents();
	virtual int fileHeaderDescribeEvents();
	virtual int fileHeaderEnd();

	virtual void setProperty(const std::string, const std::string);
	virtual string default_stream();
protected:
	static double timeDiff(const timeval &f, const timeval &s);
	static double doubleTime(const timeval &f);

	toh_outputState outputState;	// current file content status
	bool verbose;			// header output level

	/// text-specific entries for converted property values of the acqTextOutput class.
	/// This is pointed to by the 'private' member of the acqOutputEvent structure.
	class eventPrivate {
		public:
		eventPrivate() : derivedPrivate(NULL), timeStamp(0), relTimeStamp(0), rel0TimeStamp(0), commentPrefix(0), putEventID(0) {};
		void *derivedPrivate;		// available for a class derived from acqTextOutput
		struct timeval startTime,	// time when event 'start' called
			prevTime;		// timestamp of last event - initialized to 'startTime'
		bool timeStamp;
		bool relTimeStamp;
		bool rel0TimeStamp;
		bool commentPrefix;
		bool putEventID;
	};

	/// text-specific properties for PV's being output.
	class pvPrivate {
		public:
		pvPrivate() : colp(NULL), first(0), last(0), ready(0), format(NULL) {};
		virtual ~pvPrivate() {};

		virtual int output( acqKey_t key, int dataType, const void *value, int count);

		// WARNING: this points to some memory that can get reallocated!
		acqOutputColumn *colp;		// pointer to matching structure from baseHandler

		std::map<std::string,std::string> pvproperties;
		unsigned int first, last;
		int ready;
		char *format;			// keep the output format quickly available
	};

	virtual void getPrivate(int eventID, int pvID);	// map info allocator - derived classes should over-ride
	// convert the two ints for event and pvno to a single value.
	static int makeuid(int eventno, int pvno) { return (eventno<<16)|pvno;};
	map<int, pvPrivate*> pvInfo;

	acqOutputColumn * getColumn(int eventID, int pvID)
	{
		acqOutputEvent *evp = find_event_number(eventID);
		if( evp && pvID >=0 && pvID < evp->nColumn)
			return &evp->column[pvID];
		return NULL;
	};

};

#endif

#endif
