/*
 * Copyright 2007 Canadian Light Source, Inc. All Rights Reserved.
 */
/*
 * default Text Output data stream
 *
 * apologies in advance for the mixture of 'spectra' and 'spectrum'. I tried keeping
 * 'spectra' when referencing the data file (where multiple spectrum exist), and
 * 'spectrum' when referencing a single spectra, but I don't claim all singular and plural
 * references make sense.
 */

// Changed to ifndef and appened DACQLIB_ prefix (David Chevrier, Oct 27 2011)
#ifndef DACQLIB_ACQTEXTSPECTRUMOUTPUT_H
#define DACQLIB_ACQTEXTSPECTRUMOUTPUT_H 1
//#if !defined(ACQTEXTSPECTRUMOUTPUT_H)
//#define ACQTEXTSPECTRUMOUTPUT_H 1
// Changed the locash so this is up one directory (David Chevrier, Oct 27 2011)
//#include "acqDataHandler.h"
#include "../acqDataHandler.h"

#if defined(__cplusplus)
extern "C" {
#endif

acqKey_t new_acqTextOutput(void);
#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)

#include "acqTextOutput.h"
#include <string>
#include <vector>

using namespace std;

// The state of the data output file: if there is no open file, then
// the state is TOH_INIT. If there is an open file, then TOH_EMPTY
// means the file has been created, but nothing has been written.
// TOH_HAS_CONTENT means that output to the file has occured.
//
enum tsoh_outputState { TSOH_INIT, TSOH_EMPTY, TSOH_HAS_CONTENT };


class acqTextSpectrumOutput : public acqTextOutput {
public:
	enum spectrumFileEnum { SF_DEFAULT, SF_BINARY, SF_TEXT };		// the data format written
	enum spectrumSplitEnum { SS_ONE_TO_ONE, SS_BY_RECORD, SS_BY_INSTANCE };	// if writing to files, how to divide the spectra
	std::string spectrumPropertyPrefix;

private:
	int needSpectrumDelimiter;	// true when the next spectrum should be preceded by a delimiter
	std::string spectrumDelimiter;	// text placed between individual values within a spectrum.
	bool haveSpectrum;		// true if there are outputs to the spectrum file
	spectrumFileEnum spectrumFormat;		// 0=no file, 1=binary, 2=text
	acqBaseStream *spectrumStream;	// supports anadditional output stream.
	enum tsoh_outputState ts_outputState;
// Declared protected to use in a subclass (David Chevrier, Oct 27 2011)
protected:
	spectrumSplitEnum spectrumSplit;	// output file splitting - undefined results for none-file output
private:
	std::string spectrumFileName;
	std::string outputFullPath;		// output path to the destination target file.

public:
	acqTextSpectrumOutput();
	virtual ~acqTextSpectrumOutput();
	static acqBaseOutput * new_acqTextSpectrumOutput();
	bool haveSpectrumFlag() { return haveSpectrum; };

	void setSpectrumDelimiter(const std::string &delim) { spectrumDelimiter = delim; } ;
	const std::string& getSpectrumDelimiter() { return spectrumDelimiter; } ;

	void setSpectrumFormat(const spectrumFileEnum sf) { spectrumFormat = sf; };
	spectrumFileEnum getSpectrumFormat() { return spectrumFormat; };

	// callbacks from data acquisition to output handler

	static int startRepeat( acqKey_t key, int passno);
	static int start( acqKey_t key);
	static int shutdown( acqKey_t key);
	static int startRecord( acqKey_t key, int eventno);
	static int endRecord( acqKey_t key, int eventno);
	static int pvFlags( acqKey_t key, int eventno, int pvno, const char *flags);
	static int nextOutput( acqKey_t key);

	// output to the Spectrum Stream
	int sendSpectrumLine(const char * format, ...);
	int sendSpectrumData(const void *data, unsigned int dataSize);

	// routines to output file header sections
	virtual int fileHeaderStart();
	virtual int spectrumFileHeaderStart();

	// property manipulation - working with a custom stream requires supplementing
	// the property calls to allow communication with the stream.
	virtual void setProperty(const std::string, const std::string);
	virtual std::string &getProperty(std::string);
	virtual const std::string getPropertyList();
	virtual bool validProperty(const std::string);
	virtual string default_stream();
	int recordCount;

protected:

	bool verbose;			// header output level

	/// spectra-specific properties for PV's being output.
	class pvTSPrivate : public acqTextOutput::pvPrivate {
		public:
		virtual int output(acqKey_t key, int dataType, const void *value, int count);
		bool isSpectrum;
	};
	virtual void getPrivate(int eventID, int pvID);		// over-ride the map table allocator
	virtual std::string nextSpectrumFile(int record=-1, int pvid=-1);

};

#endif

///
/// Properties recognized by this handler
///
#define TEXTS_PROP_SPECTRUM "Text Spectrum Type"
#define TEXTS_NAME_SPECTRUM "Spectrum File Name Prefix"
#define TEXTS_SPECTRUM_SPLIT "Spectrum File Split"
#define TEXTS_SPECTRUM_STREAM "Spectrum Stream Type"

#endif
