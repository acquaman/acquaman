#ifndef DACQLIB_QEPICSADVACQ_H
#define DACQLIB_QEPICSADVACQ_H

#include "acquisitionLib.h"
#include "acqLibHelper.h"
#include <iostream>
#include "qepicsacqlocal.h"
#include <qregexp.h>

#include <qdebug.h>
//#include "../estimatescantime.h"

class QEpicsAcqRegion
{
	public:
		QEpicsAcqRegion(QString pv, double start, double delta, double end, double intTime, bool enable){_pv = pv; _start = start; _delta = delta; _end = end; _intTime = intTime; _enable = enable; _offset = 0;}

		QString _pv;
		double _start;
		double _delta;
		double _end;
		double _intTime;
		bool _enable;
		int _offset;
};

class QEpicsAdvAcq : public QObject
{
	Q_OBJECT

	public:
		QEpicsAdvAcq(QEpicsAcqLocal *acq);
		QString getConfigFile(){return _acq->ConfigFile();}
		void showMode(acqState mode){_acq->showMode(mode);}
		void showCompletion( QString status, int completion){_acq->showCompletion(status, completion);}
		double getValue( char *scanName, char *field, int rowNumber){return _acq->getValue(scanName, field, rowNumber);}
		int setValue( char *scanName, char *field, int rowNumber, double value){return _acq->setValue(scanName, field, rowNumber, value);}
		int setValue( char *scanName, char *field, double value){return _acq->setValue(scanName, field, value);}
		void outFileChanged(){_acq->outFileChanged();}
		int getSeqNo(void){return _acq->getSeqNo();}
		int getRuns(void){return _acq->getRuns();}
		int pvIndex( const char *event, const char *PVName){return _acq->pvIndex(event, PVName);}
		acqState getState() { return _acq->getState(); }
		QEpicsAcqLocal *getAcq() { return _acq; }
		acqMaster_t *getMaster() { return _acq->getMaster(); }

		const QString &getStatus() { return _acq->getStatus(); }
		int getCompletion() { return _acq->getCompletion(); }
		int isRunning() { return _acq->isRunning(); }

		const QString &getCommentString() { return _acq->getCommentString(); }

		QString getPV(int region);
		double getStart(int region);
		QString getStrStart(int region);
		double getDelta(int region);
		QString getStrDelta(int region);
		double getEnd(int region);
		QString getStrEnd(int region);
		double getIntTime(int region);
		QString getStrIntTime(int region);
		double getDefaultIntTime(){return _defaultIntTime;}
		QString getStrDefaultIntTime(){QString tmpStr = ""; return tmpStr.setNum(_defaultIntTime);}
		double getMaxIntTime(){return _maxIntTime;}
		double getE0(){return _E0;}
		double getKMin(){return _kmin;}
		double getKMax(){return _kmax;}
		double getKTresh(){return _ktresh;}
		QString getStrMaxIntTime(){QString tmpStr = ""; return tmpStr.setNum(_maxIntTime);}
		bool isEnable(int region);
		bool hasNullStart(int region){if( (_regions.at(region)->_start == 0) && (region != 0) ){return TRUE;}else{return FALSE;}}
		int getHasStart(int region){if(sp){return (sp->acqControlList[region]).haveStartVal;}else{return 0;}}
		int getNumRegions(){return _regions.count();}
		int getNumRecords(){return first_acqEvent(getMaster())->numPvList;}
		QString getQuickInputs();
//        QString getOutputFile(){return getMaster()->acqFile->fileTemplate;}
//        QString getOutputPath(){return getMaster()->acqFile->dirname;}


//        void registerScan(estimateScan *estScan){scan = estScan;}

		bool addRegion(int region);
		bool addRegion(int region, QString pv, double start, double delta, double end, double intTime);
		bool addRegion(int region, double start, double delta, double end, double intTime);
		bool addRegion(int region, double delta, double end, double intTime);
		bool deleteRegion(int region);
		bool clearRegions();
		bool addRecord(int record);
		bool addRecord(int record, QString pv, bool enable, bool spectrum, int mode);
		bool appendRecord(QString pv, bool enable, bool spectrum, int mode);
		bool deleteRecord(int record);

		void setValidateInputFn( QString (*newValidateInput)(QEpicsAdvAcq *acq, int region, QString field, QString input, QString &error) ){ validateInput = newValidateInput;}
		void setValidateConsequenceFn( void (*newValidateConsequence)(QEpicsAdvAcq *acq, int region, QString field, QString input) ){ validateConsequence = newValidateConsequence;}
		void setFormatOutputFn( QString (*newFormatOutput)(QEpicsAdvAcq *acq, int region, QString field, QString input) ){ formatOutput = newFormatOutput;}

		void spit();

	public slots:
		void setPV(int region, QString pvname);
		void setStart(int region, double start);
		void setStrStart(int region, QString start);//{std::cout << "Input was " << start << " now is " << (*validateInput)(start) << "\n";fflush(stdout);}
		void setDelta(int region, double delta);
		void setStrDelta(int region, QString delta);
		void setEnd(int region, double end);
		void setStrEnd(int region, QString end);
		void setIntTime(int region, double intTime);
		void setDefaultIntTime(double intTime){_defaultIntTime = intTime;}
		void setMaxIntTime(double intTime){_maxIntTime = intTime;}
		void setE0(double E0){_E0 = E0;}
		void setKMin(double kmin){_kmin = kmin;}
		void setKMax(double kmax){_kmax = kmax;}
		void setKTresh(double ktresh){_ktresh = ktresh;}
		void setEnable(int region, bool enable);
		void setHasStart(int region, int val){if(sp){(sp->acqControlList[region]).haveStartVal=val;}}
		bool setQuickInputs(QString inputs);
		void saveConfigFile(const QString &infile){char* SAVEFILE = const_cast<char*>(infile.toAscii().data()); acq_file_save_as(strdup(SAVEFILE), getMaster());}

		bool setConfigFile( const QString &filename) {
			_acq->setConfigFile(filename);
			bool retVal = buildFromConfig();
			if(retVal)
				emit onConfigLoaded();
			return retVal;
		}
		void Start() { _acq->Start(); }
		void Pause(int mode) { _acq->Pause(mode); }
		void Stop() { _acq->Stop(); }
		void Toggle( bool start) { _acq->Toggle(start); }
		void setVariable( const QString &varName, const QString &value) { _acq->setVariable(varName, value); }
		void setVariable( const QString &varName, const double value) { _acq->setVariable(varName, value); }
		void defVariable( const QString &varName, const QString &scan, const QString &type, int varIndex) { _acq->defVariable(varName, scan, type, varIndex=0); }
//        void outputFile( const QString& fileName) { _acq->outputFile(fileName); }
//        void outputDirectory(const QString& dirName) { _acq->outputDirectory(dirName); }
		void outputMessage( const QString & msg) { _acq->outputMessage(msg); }
		void outputOnelineMessage( const QString & olmsg) { _acq->outputOnelineMessage(olmsg); }
		void setSeqNo( int seq) { _acq->setSeqNo(seq); }
		void setRuns(int runs) { _acq->setRuns(runs); }
		void setCommentString(const QString &str) { _acq->setCommentString(str); }

	signals:
		void onNewConfig();
		void onConfigLoaded();
		void invalidInput(const QString &error);
		// CAUTION: these signals are triggered from an independent thread,
		// and can cause problems is widget updating occurs from multiple
		// threads at multiple times!
		void onStart();
		void onPause(int pause);
		void onStop();
		void onState(const QString&);
		void sendStatus(const QString&);
		void sendCompletion(int);
		void nextOutputFile(const QString&);
		void changeRunNumber(int);
		void sendMessage( const QString &);
		void sendOnelineMessage( const QString &);

	private:
		QEpicsAcqLocal *_acq;
//        QPtrList<QEpicsAcqRegion> _regions;
		QList<QEpicsAcqRegion*> _regions;
		double _defaultIntTime;
		double _maxIntTime;
		double _E0, _kmin, _kmax, _ktresh;

//        estimateScan *scan;
		acqScan_t *sp;
		acqControl_t ctls;

		bool buildFromConfig();
		void makeEmptyCTLS();

		QString (*validateInput)(QEpicsAdvAcq *acq, int region, QString field, QString input, QString &error);
		void (*validateConsequence)(QEpicsAdvAcq *acq, int region, QString field, QString input);
		QString (*formatOutput)(QEpicsAdvAcq *acq, int region, QString field, QString input);
};

#endif

