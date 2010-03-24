// $Header:$
// Copyright Canadian Light Source, Inc. All rights reserved.
//
// Description:
//
//

#ifndef QEPICSACQ_H
#define QEPICSACQ_H
#include <qwidget.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qmutex.h>
#include "epicsConnect.h"
#include "acquisitionLib.h"
//#include "acqTextOutput.h"
#include "OutputHandler/acqTextOutput.h"
#include "qepicsacqclass.h"


// typedef void (*putModeFunc_t)( const acqMaster_t *master, const acqState st);

class QEpicsAcqLocal : public QEpicsAcqClass
{
	Q_OBJECT

public:
	static QEpicsAcqClass *buildMe( QWidget *parent, const char *name);
	QEpicsAcqLocal( QWidget *parent = 0, const char *name = 0);
	virtual ~QEpicsAcqLocal();

	virtual QString ConfigFile() const;
	virtual void showCompletion( QString status, int completion);
	virtual double getValue( char *, char *, int);
	virtual int setValue( char *, char *, int, double);
	virtual int setValue( char *, char *, double);
	virtual void outFileChanged();
	virtual int getSeqNo(void);
	virtual int getRuns(void);
	virtual int pvIndex( const char *, const char *);
	virtual const char * getVariable(const char *);
	virtual int setVariable( const char *, const char*);
	virtual bool deleteControlLine( char *, int);
	virtual bool deleteControl(char *);
	virtual int addControlLine( char *);
	virtual bool setControlLine( char *, int, double first=NAN, double last=NAN, double increment=NAN);
	virtual void outputHandlerRegister( eventDataHandler *, const acqKey_t);
	virtual void outputHandlerRemove(const acqKey_t);

	virtual acqMaster_t *getMaster() { return master; };

	acqState getState();
	void showMode( acqState);

	const QString &getStatus() { return lastStatus; }; 
	int getCompletion() { return lastCompletion; };
	virtual int isRunning() { return running; };
	virtual void setDebug(int dblevel) { if(master) master->acqDebug = dblevel; };

	const QString &getCommentString() { return commentString ; } ;

public slots:
	virtual void setConfigFile( const QString &filename);
	virtual void Start();
	virtual void Pause(int mode);
	virtual void Stop();
	virtual void Toggle( bool start);
	virtual void setVariable( const QString &varName, const QString &value);
	virtual void setVariable( const QString &varName, const double value);
	virtual void defVariable( const QString &varName, const QString &scan, const QString &type, int varIndex=0);
	virtual void outputMessage( const QString &);
	virtual void outputOnelineMessage( const QString &);
	virtual void clearMessageBox();
	virtual void setSeqNo( int);
	virtual void setRuns(int);
	virtual void setCommentString(const QString &str) { commentString = str; } ;

signals:
	void onHandlerSignal(void *, unsigned int, void *);
	
private slots:
	// caution - signal generation can be initialized from various threads, and the timer is
	// used to force signals to only occur in the main thread.
	void onThreadTimer();		// tick to keep the signals in a single thread.
	void onMonitorTimer();		// tick to check status when running
private:
	bool running;			// TRUE if running
	bool threadSafe;		// only allow 'emit' from base thread
	QTimer threadTimer;		// emit manager.
	QTimer monitorTimer;		// manages the acq_monitor() calls
	QMutex threadTimerLock;		// protect the variables!
	unsigned int			// flags for emits to occur:
		signalOnNewConfig:1,			// request to change the loaded configuration file
		signalOnPause:1,			// request to change the acquistion pause state
		signalOnStop:1,				// called from library that acquisition is complete
		signalOnStart:1,			// request to start an application running
		signalSendStatus:1,			// called from library with change in status
		signalSendCompletion:1,			// called from library with change in completion percentage
		signalNextOutputFile:1,			// UNUSED
		signalChangeRunNumber:1,		// UNUSED
		signalSendMessage:1,			// request to display a message string
		signalSendOnelineMessage:1,		// request to display a status string
		signalOnState:1,			// called from library with a change in operating mode
		signalSendMessageBoxClear:1,		// not an 'emit', but handled by the timer
		signalOnHandlerSignal:1,		// notification from library of handler signal
		signalOnNewProperty:1;			// notification from library of changed handler property
	int argOnPause;
	QString argSendStatus;
	int argSendCompletion;
	QString argNextOutputFile;
	int argChangeRunNumber;
	QString argSendMessage;
	QString argSendOnelineMessage;
	QString argOnState;
	QString argPropertyName;
	void * argOnHandlerFrom;
	unsigned int argOnHandlerSignal;
	

	acqState lastState;		// most recent state
	QString lastStatus;		// save the most recent displayed status
	int lastCompletion;		// save the most recent displayed completion
	acqMaster_t *master;		// internal description of configuration
	QString config_file;		// configuration file used for description
	QString commentString;		// value to print in the 'comment' field
	static void localShowMode(acqMaster_t *m, acqState mode);
	static void localShowProgress( acqMaster_t *m, const char *status, double completion);
	static void localMessageOut(acqMaster_t *master, const char *fmt, ...);
	static void localStatusOut(acqMaster_t *master, const char *fmt, ...);
	static void localMessageBoxClear(acqMaster_t *master);
	static void localOnStop(acqMaster_t *master);
	static int statusMessage( registrarKey_t, const char *message);
	static int getHandlerSignal(void *, void *, unsigned int, const void *);
	
};

#endif

// $Log: qepicsacq.h  $
// Revision 1.4.1.1 2007/05/19 12:25:37CST Glen Wright (wrightg) 
// Duplicate revision

