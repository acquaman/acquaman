// $Header:$
// Copyright Canadian Light Source, Inc. All rights reserved.
//
// Description:
//
//

#ifndef DACQLIB_QEPICSACQCLASS_H
#define DACQLIB_QEPICSACQCLASS_H
#include <qobject.h>
#include <qlabel.h>
#include <math.h>
#include "acquisitionLib.h"
#include "acqDataHandler.h"
#include "OutputHandler/acqBaseOutput.h"

#include "factoryQtTemplate.h"

/// generic class for accessing data acquisition techniques
///
class QEpicsAcqClass : public QObject
{
	Q_OBJECT

protected:
	QEpicsAcqClass( QWidget *parent = 0, const char *name = 0);
public:
	virtual QString ConfigFile() const = 0;
	virtual double getValue( char *, char *, int) = 0;
	virtual int setValue( char *, char *, int, double) = 0;
	virtual int setValue( char *, char *, double) = 0;
	virtual int getRuns(void) = 0;
	virtual int pvIndex( const char *, const char *) = 0;
	virtual const char * getVariable(const char *) = 0;
	virtual int setVariable( const char *, const char*) = 0;
	// virtual void showMode( acqState) = 0;
	// virtual void outFileChanged() = 0;
	// virtual acqState getState() = 0;

	virtual const QString &getStatus() = 0;
	virtual int getCompletion() = 0;
	virtual int isRunning() = 0;

	virtual acqMaster_t * getMaster() = 0;			// classes that don't support an acqMaster shall return NULL
	virtual bool deleteControlLine( char *, int) = 0;
	virtual bool deleteControl(char *) = 0;
	virtual int addControlLine( char *) = 0;
	virtual bool setControlLine( char *, int, double first=NAN, double last=NAN, double increment=NAN) = 0;
	virtual void outputHandlerRegister( eventDataHandler *, const acqKey_t) = 0;
	virtual void outputHandlerRemove( const acqKey_t) = 0;
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

