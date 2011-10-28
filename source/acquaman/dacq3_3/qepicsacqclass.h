// $Header:$
// Copyright 2010 Canadian Light Source, Inc. All rights reserved.
//
// Description:
//
//

// Appended DACQLIB_ Prefix (David Chevrier, Oct 27 2011)
#ifndef DACQLIB_QEPICSACQCLASS_H
#define DACQLIB_QEPICSACQCLASS_H
#include <qobject.h>
#include <qlabel.h>
#include <math.h>
// Changed to local search (David Chevrier, Oct 27, 2011)
//#include <acquisitionLib.h>
//#include <acqDataHandler.h>
//#include <acqBaseOutput.h>
#include "acquisitionLib.h"
#include "acqDataHandler.h"
#include "OutputHandler/acqBaseOutput.h"

//#include <factoryQtTemplate.h>
#include "factoryQtTemplate.h"

/// generic class for accessing data acquisition techniques
/// Supports, but not limited to, the CLS data acquisition library.
///
//
// This class is intended to allow minor modification of a loaded configuration, but it not a general editor
// for a data acquisition configuration. This allows keeping options open for other types of scanning.
// There is nothing stopping a subclass from taking on the more specific features of editing specific types of
// scans and their configurations.
// Terminology used:
// A 'scan' is a loop control. It specifies a list of control variables, and the ranges the variables run through
// a 'control line' is a single control for a 'scan'. This specifies one PV, one start value, on end value, and one increment
// an 'event' is a list of PV's to be recorded.
class QEpicsAcqClass : public QObject
{
	Q_OBJECT

protected:
	QEpicsAcqClass( QWidget *parent = 0);
public:
	virtual QString ConfigFile() const = 0;
	virtual double getValue( char *, char *, int) = 0;
	virtual int setValue( char *, char *, int, double) = 0;
	virtual int setValue( char *, char *, double) = 0;
	virtual int getRuns(void) = 0;
	virtual int pvIndex( const char *, const char *) = 0;
	virtual const char * getVariable(const char *) = 0;
	virtual int setVariable( const char *, const char*) = 0;

	virtual const QString &getStatus() = 0;
	virtual int getCompletion() = 0;
	virtual int isRunning() = 0;

	virtual acqMaster_t * getMaster() = 0;			// classes that don't support an acqMaster shall return NULL
	virtual bool deleteControlLine( char *, int) = 0;	// remove a single control element for the named scan
	virtual bool deleteControl(char *) = 0;			// remove all control elements for the named scan
	virtual int addControlLine( char *) = 0;		// add an empty control line for the named scan
	//
	// set the values for a control line. If index < 0, insert a new control line. If index > last control index, append a new control line
	virtual bool setControlLine( const char *, int, const QString &varName=QString(), double first=NAN, double last=NAN, double increment=NAN) = 0;
	virtual bool setControlLine( const QString &, int, const QString &varName=QString(), double first=NAN, double last=NAN, double increment=NAN) = 0;
	virtual bool setControlLine( const char *, int, const QString &varName=QString(), const QString &first=QString(), const QString &last=QString(), const QString &increment=QString()) = 0;
	virtual bool setControlLine( const QString &, int, const QString &varName=QString(), const QString &first=QString(), const QString &last=QString(), const QString &increment=QString()) = 0;
	virtual QStringList getScanID() = 0;			// get the list of scan names
	//
	// events
	virtual bool setEventLine(char *, int, const QString &) = 0;
	virtual bool setEventLine(const QString &, int, const QString &) = 0;
	virtual QStringList getEventID() = 0;			// get the list of event names

	// Output Handlers
	virtual void outputHandlerRegister( eventDataHandler *, const acqKey_t) = 0;
	virtual void outputHandlerRemove( const acqKey_t) = 0;
	virtual void outputHandlerRegister(const QString &handlerName, const QString &handlerLib=0) = 0;
public slots:
	virtual void setConfigFile( const QString &filename) = 0;
	virtual void Start() = 0;
	virtual void Pause(int mode) = 0;
	virtual void Stop() = 0;
	virtual void setVariable( const QString &varName, const QString &value) = 0;
	virtual void setVariable( const QString &varName, const double value) = 0;
	virtual void defVariable( const QString &varName, const QString &scan, const QString &type, int varIndex=0) = 0;
	virtual void outputMessage( const QString &) = 0;
	virtual void outputOnelineMessage( const QString &) = 0;
	virtual void clearMessageBox() = 0;
	virtual void setRuns(int) = 0;
	virtual void showCompletion( QString status, int completion) = 0;
	virtual void setCommentString( const QString &comment) = 0;
	virtual void setDebug(int debugLevel) = 0;



signals:
	void onNewConfig();
	void onStart();
	void onPause(int pause);
	void onStop();
	void onState(const QString&);
	void sendStatus(const QString&);
	void sendCompletion(int);
	void changeRunNumber(int);
	void sendMessage( const QString &);
	void sendOnelineMessage( const QString &);
	void sendMessageBoxClear();
	void newHandlerProperty(const QString &);

};

// Factory class: add different classes, register by std::string name
typedef genericFactory<QEpicsAcqClass> acqFactory;
#endif

// $Log: $
// Added header/log info

