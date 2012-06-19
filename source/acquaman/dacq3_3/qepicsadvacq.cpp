//#ifndef QEPICSADVACQ_CPP
//#define QEPICSADVACQ_CPP

#include "qepicsadvacq.h"
#include <iostream>

QString defaultDEpicsValidator(QEpicsAdvAcq *acq, int region, QString field, QString input, QString &error)
{
	Q_UNUSED(acq);
	Q_UNUSED(region);
	Q_UNUSED(field);
	bool valid = FALSE;
	input.toDouble(&valid);
	if(!valid)
	{
		error = "Could not parse input string.";
		return "";
	}
	else
		return input;
}

void defaultDEpicsConsequence(QEpicsAdvAcq *acq, int region, QString field, QString input)
{
	Q_UNUSED(acq);
	Q_UNUSED(region);
	Q_UNUSED(field);
	Q_UNUSED(input);
	return;
}

QString defaultDEpicsFormator(QEpicsAdvAcq *acq, int region, QString field, QString input)
{
	Q_UNUSED(acq);
	Q_UNUSED(region);
	Q_UNUSED(field);
	return input;
}

QEpicsAdvAcq::QEpicsAdvAcq(QEpicsAcqLocal *acq)
{
	_acq = acq;
	_defaultIntTime = 1;
	_maxIntTime = 10;
	_E0 = -1;

	setValidateInputFn(&defaultDEpicsValidator);
	setValidateConsequenceFn(&defaultDEpicsConsequence);
	setFormatOutputFn(&defaultDEpicsFormator);

	connect(_acq, SIGNAL(onNewConfig()), SIGNAL(onNewConfig()));
	connect(_acq, SIGNAL(onStart()), SIGNAL(onStart()));
	connect(_acq, SIGNAL(onPause(int)), SIGNAL(onPause(int)));
	connect(_acq, SIGNAL(onStop()), SIGNAL(onStop()));
	connect(_acq, SIGNAL(onState(const QString&)), SIGNAL(onState(const QString&)));
	connect(_acq, SIGNAL(sendStatus(const QString&)), SIGNAL(sendStatus(const QString&)));
	connect(_acq, SIGNAL(sendCompletion(int)), SIGNAL(sendCompletion(int)));
	connect(_acq, SIGNAL(changeRunNumber(int)), SIGNAL(changeRunNumber(int)));
	connect(_acq, SIGNAL(sendMessage( const QString &)), SIGNAL(sendMessage(const QString&)));
	connect(_acq, SIGNAL(sendOnelineMessage( const QString &)), SIGNAL(sendOnelineMessage(const QString &)));

	buildFromConfig();
}

QString QEpicsAdvAcq::getPV(int region)
{
	if(region >= _regions.count())
		return "";
	else
		return _regions.at(region)->_pv;
}


double QEpicsAdvAcq::getStart(int region)
{
	if(region >= _regions.count())
		return -1;
	else if( (_regions.at(region)->_start == 0) && (region != 0) )
		return _regions.at(region-1)->_end + _regions.at(region)->_delta;
	else
		return _regions.at(region)->_start;
}

QString QEpicsAdvAcq::getStrStart(int region)
{
	QString tmpStr = "";
	return (*formatOutput)(this, region, "start", tmpStr.setNum(getStart(region)));
}

double QEpicsAdvAcq::getDelta(int region)
{
	if(region >= _regions.count())
		return 0;
	else
		return _regions.at(region)->_delta;
}

QString	QEpicsAdvAcq::getStrDelta(int region)
{
	QString tmpStr = "";
	return (*formatOutput)(this, region, "delta", tmpStr.setNum(getDelta(region)));
}

double QEpicsAdvAcq::getEnd(int region)
{
	if(region >= _regions.count())
		return -1;
	else
		return _regions.at(region)->_end;
}

QString	QEpicsAdvAcq::getStrEnd(int region)
{
	QString tmpStr = "";
	return (*formatOutput)(this, region, "end", tmpStr.setNum(getEnd(region)));
}

double QEpicsAdvAcq::getIntTime(int region)
{
	if(region >= _regions.count())
		return -1;
	else
		return _regions.at(region)->_intTime;
}

QString	QEpicsAdvAcq::getStrIntTime(int region)
{
	QString tmpStr = "";
	if(getIntTime(region) == 66276)
		return "f";
	else
		return tmpStr.setNum(getIntTime(region));
}

QString QEpicsAdvAcq::getQuickInputs()
{
	static const QRegExp kspace(":[kK]");
	bool isFirst = TRUE;
	QString boundaries, steps, integrations, tempStr;
	integrations = QString::null;
	for(int x = 0; x < _regions.count(); x++)
	{
		if(!isEnable(x))
			continue;
		if(isFirst)
		{
			boundaries = getStrStart(x);
			boundaries += " ";
			isFirst = FALSE;
			steps = QString::null;
			boundaries += getStrEnd(x);
			boundaries += " ";
			steps += getStrDelta(x);
			steps += " ";
		}
		else
		{
			if( (getPV(x).contains(kspace) > 0) && (getPV(x-1).contains(kspace) == 0) )
				std::cout << "It's " << getStart(x) << "K->" << 3.81*getStart(x)*getStart(x)+getE0() << " " << getEnd(x-1) << " " << getDelta(x) << "K->" << 3.81*getDelta(x)*getDelta(x)+getE0() << "\n";
//            if( (getPV(x).contains(kspace) > 0) && (getPV(x-1).contains(kspace) == 0) && (fabs(3.81*getStart(x)*getStart(x)+getE0() - getEnd(x-1)) < 0.01) )
			if( (getPV(x).contains(kspace) > 0) && (getPV(x-1).contains(kspace) == 0) && (fabs(getStart(x) - sqrt( (getEnd(x-1)-getE0())/3.81 - getDelta(x)) ) < 0.01) )
			{
				boundaries += getStrEnd(x);
				boundaries += " ";
				steps += getStrDelta(x);
				steps += " ";
			}
			else if((getPV(x).contains(kspace) > 0) && (getPV(x-1).contains(kspace) == 0) && (fabs(getStart(x) - sqrt( (getEnd(x-1)-getE0())/3.81) -getDelta(x)) >= 0.01) )
			{
				boundaries += getStrStart(x);
				boundaries += " ";
				boundaries += getStrEnd(x);
				boundaries += " ";
				tempStr.setNum(3.81*getStart(x)*getStart(x)+getE0() - getEnd(x-1));
				steps += tempStr;
				steps += " ";
				steps += getStrDelta(x);
				steps += " ";
			}
			else if( fabs(getStart(x) - getEnd(x-1) - getDelta(x)) < 0.01 )
			{
				boundaries += getStrEnd(x);
				boundaries += " ";
				steps += getStrDelta(x);
				steps += " ";
			}
			else
			{
				boundaries += getStrStart(x);
				boundaries += " ";
				boundaries += getStrEnd(x);
				boundaries += " ";
				tempStr.setNum(getStart(x) - getEnd(x-1));
				steps += tempStr;
				steps += " ";
				steps += getStrDelta(x);
				steps += " ";
			}
	}
		integrations += getStrIntTime(x);
		integrations += " ";
	}
	return boundaries+"^"+steps+"^"+integrations;
}

bool QEpicsAdvAcq::isEnable(int region)
{
	if(region >= _regions.count())
		return FALSE;
	else
		return _regions.at(region)->_enable;
}

bool QEpicsAdvAcq::addRegion(int region)
{
	int sz, tmpOffset;
	tmpOffset = 0;
	if ( region > _regions.count() )
		return FALSE;
	else if ( sp->numControlPV == 1 && sp->acqControlList->controlPV == NULL )
	{
		sp->acqControlList[0] = ctls;
		sp->acqControlList[region].controlPV = strdup(ctls.controlPV);
	}
	else
	{
		sp->numControlPV++;
		sz = sizeof (acqControl_t)*sp->numControlPV;
		sp->acqControlList = (acqControl_t*)realloc( sp->acqControlList, sz);
		if( (region != _regions.count()) && (region != 0) )
			tmpOffset = _regions.at(region)->_offset;
		for( int x = sp->numControlPV-1; x > region-tmpOffset; x--)
			sp->acqControlList[x] = sp->acqControlList[x-1];
		sp->acqControlList[region-tmpOffset] = ctls;
		sp->acqControlList[region-tmpOffset].controlPV = strdup(ctls.controlPV);
	}
	QEpicsAcqRegion *tempRegion;
	QString ipv;
	double istart, idelta, iend, iintTime;
	if((sp->acqControlList[region-tmpOffset]).controlPV != NULL )
		ipv = (sp->acqControlList[region-tmpOffset]).controlPV;
	else
		ipv = _regions.at(region-1)->_pv;
	istart = _acq->getValue(sp->scanName, QString("start").toAscii().data(), region);
	idelta = _acq->getValue(sp->scanName, QString("delta").toAscii().data(), region);
	iend = _acq->getValue(sp->scanName, QString("end").toAscii().data(), region);
	iintTime = _defaultIntTime;
	tempRegion = new QEpicsAcqRegion(ipv, istart, idelta, iend, iintTime, TRUE);
	tempRegion->_offset = tmpOffset;
	_regions.insert(region, tempRegion);
	makeEmptyCTLS();
	return TRUE;
}

bool QEpicsAdvAcq::addRegion(int region, QString pv, double start, double delta, double end, double intTime)
{
	int sz, tmpOffset;
	tmpOffset =	0;
	if ( region > _regions.count() )
		return FALSE;

	ctls.controlPV = strdup(pv.toAscii().constData());

	ctls.startVal = start;
	ctls.deltaVal = delta;
	ctls.finalVal = end;
	if ( sp->numControlPV == 1 && sp->acqControlList->controlPV == NULL )
	{
		sp->acqControlList[0] = ctls;
		sp->acqControlList[region].controlPV = strdup(ctls.controlPV);
	}
	else
	{
		sp->numControlPV++;
		sz = sizeof (acqControl_t)*sp->numControlPV;
		sp->acqControlList = (acqControl_t*)realloc( sp->acqControlList, sz);
		if( (region != _regions.count()) && (region != 0) )
			tmpOffset = _regions.at(region)->_offset;
		for( int x = sp->numControlPV-1; x > region-tmpOffset; x--)
			sp->acqControlList[x] = sp->acqControlList[x-1];
		sp->acqControlList[region-tmpOffset] = ctls;
		sp->acqControlList[region-tmpOffset].controlPV = strdup(ctls.controlPV);
	}
	QEpicsAcqRegion *tempRegion;
	QString ipv;
	double istart, idelta, iend, iintTime;
	if((sp->acqControlList[region-tmpOffset]).controlPV != NULL )
		ipv = (sp->acqControlList[region-tmpOffset]).controlPV;
	else
		ipv = _regions.at(region-1)->_pv;
	istart = _acq->getValue(sp->scanName, QString("start").toAscii().data(), region);
	idelta = _acq->getValue(sp->scanName, QString("delta").toAscii().data(), region);
	iend = _acq->getValue(sp->scanName, QString("end").toAscii().data(), region);
	iintTime = intTime;
	tempRegion = new QEpicsAcqRegion(ipv, istart, idelta, iend, iintTime, TRUE);
	tempRegion->_offset = tmpOffset;
	_regions.insert(region, tempRegion);
	makeEmptyCTLS();
	return TRUE;
}

bool QEpicsAdvAcq::addRegion(int region, double start, double delta, double end, double intTime)
{
	int tmpOffset;
	tmpOffset =	0;
	if ( region > _regions.count() )
		return FALSE;
	else if( sp->numControlPV == 1 && sp->acqControlList->controlPV == NULL )
		return FALSE;
	else if( region == 0 )
		return FALSE;
	else
	{
		QString ipv = "";
		if( (region != _regions.count()) && (region != 0) )
			tmpOffset = _regions.at(region)->_offset;
		/* CHECK THIS OUT DAVE:
		   Killing the not NULL part only going with the last region idea ... is this good?
		   Don't delete this comment
		if((sp->acqControlList[region-tmpOffset]).controlPV != NULL ){
			qdebug() << "Claims NOT NULL in no name addRegion"; ipv = (sp->acqControlList[region-tmpOffset]).controlPV;}
		else{
			qdebug() << "Claims IS NULL in no name addRegion"; ipv = _regions.at(region-1)->_pv;}
		*/
		ipv = _regions.at(region-1)->_pv;
		return addRegion(region, ipv, start, delta, end, intTime);
	}
}

bool QEpicsAdvAcq::addRegion(int region, double delta, double end, double intTime)
{
	int tmpOffset;
	tmpOffset =	0;
	if ( region > _regions.count() )
		return FALSE;
	else if( sp->numControlPV == 1 && sp->acqControlList->controlPV == NULL )
		return FALSE;
	else if( region == 0 )
		return FALSE;
	else
	{
		QString ipv;
		if( (region != _regions.count()) && (region != 0) )
			tmpOffset = _regions.at(region)->_offset;
		if((sp->acqControlList[region-tmpOffset]).controlPV != NULL )
			ipv = (sp->acqControlList[region-tmpOffset]).controlPV;
		else
			ipv = _regions.at(region-1)->_pv;

		//FIX ME PLEASE!!!! havestartval = 0 ????

		return addRegion(region, ipv, 0, delta, end, intTime);
	}
}

bool QEpicsAdvAcq::deleteRegion(int region)
{
	if ( region > _regions.count() )
		return FALSE;
	deleteScanControl(sp, region-_regions.at(region)->_offset);
	delete _regions.takeAt(region);
	return TRUE;
}

bool QEpicsAdvAcq::clearRegions()
{
	while(getNumRegions() > 0)
		deleteRegion(0);
	return TRUE;
}

bool QEpicsAdvAcq::addRecord(int record)
{
	acqEvent_t *ev;
	ev = first_acqEvent(_acq->getMaster());
	if(!ev || !ev->numPvList || (record > ev->numPvList) )
		return FALSE;
	addIndexEventPv( ev, record, QString("New").toAscii().data(), 0, NULL, useCurrent, 0 );
	return TRUE;
}

bool QEpicsAdvAcq::addRecord(int record, QString pv, bool enable, bool spectrum, int mode)
{
	acqEvent_t *ev;
	ev = first_acqEvent(_acq->getMaster());
	if(!ev || !ev->numPvList || (record > ev->numPvList) )
		return FALSE;
	switch(mode)
	{
		case 0:{
			addIndexEventPv( ev, record, pv.toAscii().data(), !enable, NULL, useCurrent, spectrum );
			break;}
		case 1:{
			addIndexEventPv( ev, record, pv.toAscii().data(), !enable, NULL, usePvGet, spectrum );
			break;}
		case 2:{
			addIndexEventPv( ev, record, pv.toAscii().data(), !enable, NULL, waitForMonitor, spectrum );
			break;}
	}
	return TRUE;
}

bool QEpicsAdvAcq::appendRecord(QString pv, bool enable, bool spectrum, int mode)
{
	acqEvent_t *ev;
	ev = first_acqEvent(_acq->getMaster());
	if(!ev || !ev->numPvList )
		return FALSE;

	QString format = "%g";
	if(spectrum)
		format = "";

	switch(mode)
	{
		case 0:{
			addEventPv( ev, pv.toAscii().data(), !enable, format.toAscii().data(), useCurrent, spectrum );
			break;}
		case 1:{
			addEventPv( ev, pv.toAscii().data(), !enable, format.toAscii().data(), usePvGet, spectrum );
			break;}
		case 2:{
			addEventPv( ev, pv.toAscii().data(), !enable, format.toAscii().data(), waitForMonitor, spectrum );
			break;}
	}

	return TRUE;
}

bool QEpicsAdvAcq::deleteRecord(int record)
{
	acqEvent_t *ev;
	ev = first_acqEvent(_acq->getMaster());
	if(!ev || !ev->numPvList || (record >= ev->numPvList) )
		return FALSE;
	deleteEventPv( ev, record );
	return TRUE;
}

bool QEpicsAdvAcq::buildFromConfig()
{
	sp = first_acqScan(_acq->getMaster());
	if(sp == NULL)
		return false;
	while(_regions.count() > 0)
		delete _regions.takeAt(_regions.count()-1);
	if ( sp->numControlPV == 1 && sp->acqControlList->controlPV == NULL )
		return false;
	QEpicsAcqRegion *tempRegion;
	QString pv;
	double start, delta, end, intTime;
	for(int x = 0; x < sp->numControlPV; x++)
	{
		if((sp->acqControlList[x]).controlPV != NULL )
			pv = (sp->acqControlList[x]).controlPV;
		else
			pv = _regions.at(x-1)->_pv;
		start = _acq->getValue(sp->scanName, QString("start").toAscii().data(), x);
		delta = _acq->getValue(sp->scanName, QString("delta").toAscii().data(), x);
		end = _acq->getValue(sp->scanName, QString("end").toAscii().data(), x);
		intTime = _defaultIntTime;
		tempRegion = new QEpicsAcqRegion(pv, start, delta, end, intTime, TRUE);
		_regions.append(tempRegion);
	}

	makeEmptyCTLS();
	return true;
}

void QEpicsAdvAcq::makeEmptyCTLS()
{
	memset(&ctls, 0, sizeof ctls);

	ctls.controlPV = strdup("InsertPVToControlHere"); //MB: MEMORY LEAK HERE
	ctls.startVal = 0;
	ctls.haveStartVal=1;
	ctls.finalVal = 0;
	ctls.haveFinalVal=1;
	ctls.deltaVal = 0;
	ctls.haveDeltaVal=1;
}

void QEpicsAdvAcq::setPV(int region, QString pvname)
{
	_regions.at(region)->_pv = pvname;
	if(_regions.at(region)->_enable)
		(sp->acqControlList[region-_regions.at(region)->_offset]).controlPV = strdup(pvname.toAscii().constData());
	(*validateConsequence)(this, region, "pv", pvname);
}

void QEpicsAdvAcq::setStart(int region, double start)
{
	_regions.at(region)->_start = start;
	if(_regions.at(region)->_enable)
		setValue(sp->scanName, QString("start").toAscii().data(), region-_regions.at(region)->_offset, start);
}

void QEpicsAdvAcq::setStrStart(int region, QString start)
{
	QString lError = "";
	QString validStart = (*validateInput)(this, region, "start", start, lError);
	if(QString::compare(validStart, "") == 0)
	{
		emit invalidInput(lError);
		return;
	}
	else if(QString::compare(validStart, start) != 0)
	{
		(*validateConsequence)(this, region, "start", start);
	}
	setStart(region, validStart.toDouble());
}

void QEpicsAdvAcq::setDelta(int region, double delta)
{
	_regions.at(region)->_delta = delta;
	if(_regions.at(region)->_enable)
		setValue(sp->scanName, QString("delta").toAscii().data(), region-_regions.at(region)->_offset, delta);
}

void QEpicsAdvAcq::setStrDelta(int region, QString delta)
{
	QString lError = "";
	QString validDelta = (*validateInput)(this, region, "delta", delta, lError);
	if(QString::compare(validDelta, "") == 0)
	{
		emit invalidInput(lError);
		return;
	}
	else if(QString::compare(validDelta, delta) != 0)
	{
		(*validateConsequence)(this, region, "delta", delta);
	}
	setDelta(region, validDelta.toDouble());
}

void QEpicsAdvAcq::setEnd(int region, double end)
{
	_regions.at(region)->_end = end;
	if(_regions.at(region)->_enable)
		setValue(sp->scanName, QString("end").toAscii().data(), region-_regions.at(region)->_offset, end);
}

void QEpicsAdvAcq::setStrEnd(int region, QString end)
{
	QString lError = "";
	QString validEnd = (*validateInput)(this, region, "end", end, lError);
	if(QString::compare(validEnd, "") == 0)
	{
		emit invalidInput(lError);
		return;
	}
	else if(QString::compare(validEnd, end) != 0)
	{
		(*validateConsequence)(this, region, "end", end);
	}
	setEnd(region, validEnd.toDouble());
}

void QEpicsAdvAcq::setIntTime(int region, double intTime)
{
	_regions.at(region)->_intTime = intTime;
}

void QEpicsAdvAcq::setEnable(int region, bool enable)
{
	int sz;
	if( (enable && _regions.at(region)->_enable) || (!enable && !_regions.at(region)->_enable)){
		return;
	}
	if(enable)
	{
		ctls.controlPV = strdup(_regions.at(region)->_pv.toAscii().constData()); //MB: MEMORY LEAK STILL HERE

		ctls.startVal = _regions.at(region)->_start;
		ctls.deltaVal = _regions.at(region)->_delta;
		ctls.finalVal = _regions.at(region)->_end;
		if ( sp->numControlPV == 1 && sp->acqControlList->controlPV == NULL )
		{
			sp->acqControlList[0] = ctls;
			if(sp->acqControlList[region].controlPV) free(sp->acqControlList[region].controlPV);
			sp->acqControlList[region].controlPV = strdup(ctls.controlPV);
		}
		else
		{
			sp->numControlPV++;
			sz = sizeof (acqControl_t)*sp->numControlPV;
			sp->acqControlList = (acqControl_t*)realloc( sp->acqControlList, sz);
		for( int x = sp->numControlPV-1; x > region; x--)
				sp->acqControlList[x] = sp->acqControlList[x-1];
			sp->acqControlList[region-_regions.at(region)->_offset] = ctls;
			sp->acqControlList[region-_regions.at(region)->_offset].controlPV = strdup(ctls.controlPV);//MB: MEMORY LEAK HERE
		}
		makeEmptyCTLS();
		_regions.at(region)->_enable = enable;
	}
	else
	{
		deleteScanControl(sp, region-_regions.at(region)->_offset);
		_regions.at(region)->_enable = enable;
	}
	for(int x = region+1; x < _regions.count(); x++)
//check me	for(uint x = region+1; x < _regions.count(); x++)
		if(enable)
			_regions.at(x)->_offset--;
		else
			_regions.at(x)->_offset++;
}

bool QEpicsAdvAcq::setQuickInputs(QString inputs)
{
	int tabx = 0;
	static const QRegExp ws("[\\s,:;]+");
	static const QColor warn("red");
	QStringList boundariesList = inputs.section('^', 0, 0).split(ws);
	QStringList stepsList = inputs.section('^', 1, 1).split(ws);
	QStringList timesList = inputs.section('^', 2, 2).split(ws);

	if((stepsList.count() + 1 != boundariesList.count()) || (stepsList.count() != timesList.count()))
	{
		return FALSE;
	}
	clearRegions();

	QString tmpPV = "";
	double tmpStart, tmpDelta, tmpEnd, tmpIntTime;
	static const QRegExp kspace(":[kK]");
	if(boundariesList[0].contains(kspace) > 0)
	{
		tmpPV = "BL1606-B1-1:k";
		tmpStart = boundariesList[0].remove(kspace).toDouble();
	}
	else
	{
		tmpPV = "BL1606-B1-1:Energy";
		tmpStart = boundariesList[0].toDouble();
	}

	for(int x = 0; x < stepsList.count(); x++)
//check me	for(uint x = 0; x < stepsList.count(); x++)
	{
		if(fabs(boundariesList[x+1].toDouble() - boundariesList[x].toDouble() - stepsList[x].toDouble()) < 0.01)
			tabx--;
		else
		{
			if(stepsList[x].contains(kspace) > 0)
			{
				tmpDelta = stepsList[x].remove(kspace).toDouble();
				tmpEnd = boundariesList[x+1].remove(kspace).toDouble();
			}
			else
			{
				tmpDelta = stepsList[x].toDouble();
				tmpEnd = boundariesList[x+1].toDouble();
			}
			if( (QString::compare(timesList[x], "f") == 0) || (QString::compare(timesList[x], "F") == 0) )
				tmpIntTime = 66276;
			else
				tmpIntTime = timesList[x].toDouble();
			addRegion(tabx, tmpPV, tmpStart, tmpDelta, tmpEnd, tmpIntTime);
			if(x+1 < stepsList.count())
			{
				if(boundariesList[tabx+1].contains(kspace) > 0)
				{
					tmpPV = "BL1606-B1-1:k";
					tmpStart = boundariesList[x+1].remove(kspace).toDouble() + stepsList[x+1].remove(kspace).toDouble();
				}
				else
				{
					tmpPV = "BL1606-B1-1:Energy";
					tmpStart = boundariesList[x+1].toDouble() + stepsList[x+1].toDouble();
				}
			}
		}
		tabx++;
	}
	return TRUE;
}

void QEpicsAdvAcq::saveConfigFile(const QString &infile)
{
	acq_file_save_as(infile.toAscii().constData(), getMaster());
}

void QEpicsAdvAcq::spit()
{
}

//#endif

