#include "SGMFastDacqScanController.h"

SGMFastDacqScanController::SGMFastDacqScanController(SGMFastScanConfiguration *cfg, QObject *parent) :
		AMDacqScanController(cfg, parent), SGMFastScanController(cfg)
{
	emit scanCreated(scan());
}

void SGMFastDacqScanController::initialize(){
	if(SGMFastScanController::beamlineInitialize())
		initialized_ = true;
#warning "Do we need to also clear any raw data sources here, or just the raw data itself?"
	pScan()->clearRawDataPoints();
	emit initialized();
}

void SGMFastDacqScanController::reinitialize(bool removeScan){
	SGMFastScanController::reinitialize();
	qDebug() << "Emitting reinitialized with removeScan " << removeScan;
	emit reinitialized(removeScan);
}

void SGMFastDacqScanController::start(){
	if(SGMBeamline::sgm()->isScanning()){
		qDebug() << "Beamline already scanning";
		return;
	}
	bool loadSuccess;
	QString homeDir = QDir::homePath();
	if( QDir(homeDir+"/dev").exists())
		homeDir.append("/dev");
	else if( QDir(homeDir+"/beamline/programming").exists())
		homeDir.append("/beamline/programming");

	/*
	if(pCfg()->usingPGT())
		loadSuccess = advAcq_->setConfigFile(homeDir.append("/acquaman/devConfigurationFiles/pgt.cfg"));
	else
		loadSuccess = advAcq_->setConfigFile(homeDir.append("/acquaman/devConfigurationFiles/defaultEnergy.cfg"));
	*/
	qDebug() << "Trying to load FAST SCAN";
	loadSuccess = advAcq_->setConfigFile(homeDir.append("/acquaman/devConfigurationFiles/Scalar_Fast.config"));
	if(!loadSuccess){
		qDebug() << "LIBRARY FAILED TO LOAD CONFIG FILE";
		return;
	}
	/*
	foreach(const AMDetectorInfo *dtctr, pCfg()->usingDetectors() ){
		if(dtctr->name() == SGMBeamline::sgm()->pgtDetector()->name()){
			advAcq_->appendRecord(SGMBeamline::sgm()->pvName(dtctr->name()), true, true, 0);
		}
		else{
			advAcq_->appendRecord(SGMBeamline::sgm()->pvName(dtctr->name()), true, false, 0);
		}

		//for(int x = 0; x < pCfg()->count(); x++){
		//	if(advAcq_->getNumRegions() == x)
		//		advAcq_->addRegion(x, pCfg_()->start(x), pCfg_()->delta(x), pCfg_()->end(x), 1);
		//	else{
		//		advAcq_->setStart(x, pCfg_()->start(x));
		//		advAcq_->setDelta(x, pCfg_()->delta(x));
		//		advAcq_->setEnd(x, pCfg_()->end(x));
		//	}
		//}

	}
	*/
	generalScan_ = specificScan_;
	AMDacqScanController::start();
}

bool SGMFastDacqScanController::event(QEvent *e){
	if(e->type() == (QEvent::Type)AM::AcqEvent){
		QMap<int, double> aeData = ((AMAcqEvent*)e)->dataPackage_;
		QMap<int, QList<double> > aeSpectra = ((AMAcqEvent*)e)->spectraPackage_;
		QMap<int, double>::const_iterator i = aeData.constBegin();
		QMap<int, QList<double> >::const_iterator j = aeSpectra.constBegin();
		// Fast scan should be one scalar value (the initial energy value) and one spectral value (the scaler with all the data)
		// There will be N*1000 elements of the scaler waveform, where N is the number of channels (detectors) being acquired
		// We have already set the energy axis as uniform with the proper start and increment, so we can ignore the energy value in aeData
//		if(i.key() == 0 && (aeData.count()+aeSpectra.count()) > 1){
		if(i.key() == 0 && aeData.count() == 1 && aeSpectra.count() == 1){
			/*
			AMnDIndex insertIndex = toScanIndex(aeData);
			pScan()->rawData()->beginInsertRowsAsNecessaryForScanPoint(insertIndex);
			/// \bug CRITICAL: This is ASSUMING ONE AXIS, need to fix that somewhere
			pScan()->rawData()->setAxisValue(0, insertIndex.row(), i.value());
			++i;
			while(i != aeData.constEnd()){
				pScan()->rawData()->setValue(insertIndex, i.key()-1, AMnDIndex(), i.value());
				++i;
			}
			*/
			while(j != aeSpectra.constEnd()){
				int numPoints = j.value().at(0);
				for(int x = 0; x < j.value().count()-1; x++){
					if(x%4 == 0){
						pScan()->rawData()->beginInsertRows(0);
						pScan()->rawData()->setAxisValue(0, x/4, x/4);
						qDebug() << "NEW ROW";
					}
					qDebug() << "Column " << x%4 << " row " << x/4 << " value " << j.value().at(x+1);
					pScan()->rawData()->setValue(AMnDIndex(x/4), x%4, AMnDIndex(), j.value().at(x+1));
					if(x%4 == 3 || x == j.value().count()-2){
						pScan()->rawData()->endInsertRows();
						qDebug() << "ROW DONE";
					}
				}
				++j;
			}
			/*
			pScan()->rawData()->endInsertRows();
			*/
			qDebug() << "From data store";
			for(int x = 0; x < pScan()->rawData()->scanSize(0); x++){
				qDebug() << (double)(pScan()->rawData()->axisValue(0, x))
						<< (int)(pScan()->rawData()->value(AMnDIndex(x), 0, AMnDIndex()))
						<< (int)(pScan()->rawData()->value(AMnDIndex(x), 1, AMnDIndex()))
						<< (int)(pScan()->rawData()->value(AMnDIndex(x), 2, AMnDIndex()))
						<< (int)(pScan()->rawData()->value(AMnDIndex(x), 3, AMnDIndex()));
			}
		}
		e->accept();
		return true;
	}
	else
		return AMDacqScanController::event(e);
}


AMnDIndex SGMFastDacqScanController::toScanIndex(QMap<int, double> aeData){
	// SGM XAS Scan has only one dimension (energy), simply append to the end of this
	return AMnDIndex(pScan()->rawData()->scanSize(0));
}





SGMFastScanController::SGMFastScanController(SGMFastScanConfiguration *cfg){
	specificCfg_ = cfg;
	beamlineInitialized_ = false;

	QList<AMDetectorInfo*> scanDetectors = pCfg()->usingDetectors();

	specificScan_ = new AMFastScan();
	pScan()->setName("SGM Fast Scan");
	pScan()->setFilePath(pCfg()->filePath()+pCfg()->fileName());
	pScan()->setFileFormat("sgm2010Fast");

	// The energy axis will be slightly miscalibrated, but can be approximated as uniform with 1000 points between the
	//  start energy and the final energy
	pScan()->rawData()->scanAxisAt(0).isUniform = true;
	pScan()->rawData()->scanAxisAt(0).start = pCfg()->start();
	pScan()->rawData()->scanAxisAt(0).increment = (double) ((pCfg()->end() - pCfg()->start())/1000.0);

	qDebug() << pScan()->rawData()->scanAxisAt(0).isUniform << (double)pScan()->rawData()->scanAxisAt(0).start << (double)pScan()->rawData()->scanAxisAt(0).increment;

	// Create space in raw data store, and create raw data channels, for each detector.

	qDebug() << "Count is " << scanDetectors.count();
	for(int i=0; i<scanDetectors.count(); i++) {
		qDebug() << "Detector at " << i << " is " << scanDetectors.at(i)->name() ;
		AMDetectorInfo* detectorInfo = scanDetectors.at(i);
		pScan()->rawData()->addMeasurement(AMMeasurementInfo(*detectorInfo));
		pScan()->addRawDataSource(new AMRawDataSource(pScan()->rawData(), i));
	}

	QList<AMDataSource*> raw1DDataSources;
	for(int i=0; i<pScan()->rawDataSources()->count(); i++)
		if(pScan()->rawDataSources()->at(i)->rank() == 1)
			raw1DDataSources << pScan()->rawDataSources()->at(i);

	int rawTeyIndex = pScan()->rawDataSources()->indexOf("tey");
	int rawTfyIndex = pScan()->rawDataSources()->indexOf("tfy");
	int rawI0Index = pScan()->rawDataSources()->indexOf("I0");

	if(rawTeyIndex != -1 && rawI0Index != -1) {
		AM1DExpressionAB* teyChannel = new AM1DExpressionAB("tey_n");
		teyChannel->setDescription("Normalized TEY");
		teyChannel->setInputDataSources(raw1DDataSources);
		teyChannel->setExpression("tey/I0");

		pScan()->addAnalyzedDataSource(teyChannel);
	}

	if(rawTfyIndex != -1 && rawI0Index != -1) {
		AM1DExpressionAB* tfyChannel = new AM1DExpressionAB("tfy_n");
		tfyChannel->setDescription("Normalized TFY");
		tfyChannel->setInputDataSources(raw1DDataSources);
		tfyChannel->setExpression("-tfy/I0");

		pScan()->addAnalyzedDataSource(tfyChannel);
	}

	/// \bug CRITICAL Removed creating default channels. They were never set anyway (nothing called the old AMXASScan::setDefaultChannels(); )

}

bool SGMFastScanController::isBeamlineInitialized() {
	return beamlineInitialized_;
}

bool SGMFastScanController::beamlineInitialize(){
	SGMBeamline::sgm()->exitSlitGap()->move( pCfg()->exitSlitGap() );
	SGMBeamline::sgm()->grating()->move( pCfg()->grating() );
	SGMBeamline::sgm()->harmonic()->move( pCfg()->harmonic() );
	SGMBeamline::sgm()->undulatorTracking()->move( pCfg()->undulatorTracking() );
	SGMBeamline::sgm()->monoTracking()->move( pCfg()->monoTracking() );
	SGMBeamline::sgm()->exitSlitTracking()->move( pCfg()->exitSlitTracking() );

	AMDetectorInfo* tmpDI;
#warning "YEAH DAVE, CHECK THIS OUT! I THINK DETECTOR SET IS PROGRAMMED WRONG!!!"
	//for(int x = 0; x < pCfg()->detectorSet()->count(); x++){
	for(int x = 0; x < pCfg()->usingDetectors().count(); x++){
		//tmpDI = pCfg()->detectorSet()->detectorAt(x);
		tmpDI = pCfg()->usingDetectors().at(x);
		#warning "David please review... Had to change because of removed AMDbObject::typeDescription"
		/* previously: typeDescription()s were never the safest way to tell what class something was anyway.
		if(tmpDI->typeDescription() == "PGT SDD Spectrum-Output Detector")
			((PGTDetector*)(pCfg_()->detectorSet()->detectorAt(x)))->setControls( (PGTDetectorInfo*)pCfg_()->cfgDetectorInfoSet()->detectorAt(x) );
		else if(tmpDI->typeDescription() == "MCP Detector")
			((MCPDetector*)(pCfg_()->detectorSet()->detectorAt(x)))->setControls( (MCPDetectorInfo*)pCfg_()->cfgDetectorInfoSet()->detectorAt(x) );
		else
			((AMSingleControlDetector*)(pCfg_()->detectorSet()->detectorAt(x)))->setControls( (AMDetectorInfo*)pCfg_()->cfgDetectorInfoSet()->detectorAt(x) );
			*/
		// replaced with: use qobject_cast<toType*>(genericType*).  Returns 0 if genericType* is not of the toType type.
		PGTDetector* pgtDetector;
		MCPDetector* mcpDetector;
		AMSingleControlDetector* scDetector;

		#warning "David: I'm concerned about the cfgDetectorInfoSet and the usingDetectors not being aligned in x so you can fix it and uncomment it"
		/*
		if( (pgtDetector = qobject_cast<PGTDetector*>(tmpDI)) )
			pgtDetector->setControls( (PGTDetectorInfo*)pCfg()->cfgDetectorInfoSet()->detectorAt(x) );
		else if( (mcpDetector = qobject_cast<MCPDetector*>(tmpDI)) )
			mcpDetector->setControls( (MCPDetectorInfo*)pCfg()->cfgDetectorInfoSet()->detectorAt(x) );
		else if( (scDetector = qobject_cast<AMSingleControlDetector*>(tmpDI)) )
			scDetector->setControls( (AMDetectorInfo*)pCfg()->cfgDetectorInfoSet()->detectorAt(x) );
		*/

	}

	beamlineInitialized_ = true;
	return beamlineInitialized_;
}

void SGMFastScanController::reinitialize(){
	/// \bug CRITICAL this was commented out. Why?
	delete specificScan_;

	QList<AMDetectorInfo*> scanDetectors;
	scanDetectors = pCfg()->usingDetectors();
	//scanDetectors.prepend(SGMBeamline::sgm()->i0Detector());
	//scanDetectors.prepend(SGMBeamline::sgm()->eVFbkDetector());


	specificScan_ = new AMFastScan();
	//_pScan_ = &specificScan_;
	pScan()->setName("SGM Fast Scan");
	pScan()->setFilePath(pCfg()->filePath()+pCfg()->fileName());
	pScan()->setFileFormat("sgm2010Fast");

	// The energy axis will be slightly miscalibrated, but can be approximated as uniform with 1000 points between the
	//  start energy and the final energy
	pScan()->rawData()->scanAxisAt(0).isUniform = true;
	pScan()->rawData()->scanAxisAt(0).start = pCfg()->start();
	pScan()->rawData()->scanAxisAt(0).increment = (double) ((pCfg()->end() - pCfg()->start())/1000.0);

	// Create space in raw data store, and create raw data channels, for each detector.

	for(int i=0; i<scanDetectors.count(); i++) {
		AMDetectorInfo* detectorInfo = scanDetectors.at(i);

		pScan()->rawData()->addMeasurement(AMMeasurementInfo(*detectorInfo));
		pScan()->addRawDataSource(new AMRawDataSource(pScan()->rawData(), i));
	}

	/// \bug CRITICAL removed creating default channels. Was never used anyway.
}
