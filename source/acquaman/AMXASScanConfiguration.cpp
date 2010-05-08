#include "AMXASScanConfiguration.h"

/// Constructor simply passes arguments up to AMScanConfiguration constructor.
AMXASScanConfiguration::AMXASScanConfiguration(QObject *parent) : AMScanConfiguration(parent)
{
	regions_ = new AMXASRegionsList(this);

	/*
	QList<AMControl*> fluxresList;
	fluxresList << SGMBeamline::sgm()->exitSlitGap() << SGMBeamline::sgm()->grating() << SGMBeamline::sgm()->harmonic();
	*/

	/*
	setExitSlitGap(SGMBeamline::sgm()->exitSlitGap()->value());
	setGrating((SGMBeamline::sgm()->grating()->value() < 1) ? SGMBeamline::lowGrating : ((SGMBeamline::sgm()->grating()->value() < 2) ? SGMBeamline::mediumGrating : SGMBeamline::highGrating));
	setUndulatorTracking(SGMBeamline::sgm()->undulatorTracking()->value());
	setMonoTracking(SGMBeamline::sgm()->monoTracking()->value());
	setExitSlitTracking(SGMBeamline::sgm()->exitSlitTracking()->value());
	*/
}

double AMXASScanConfiguration::start(size_t index) const
{
	return regions_->start(index);
	/*
	AMXASRegion *xasr = region(index);
	if(xasr)
		return xasr->start();
	return -1;
	*/
}

double AMXASScanConfiguration::delta(size_t index) const
{
	return regions_->delta(index);
	/*
	AMXASRegion *xasr = region(index);
	if(xasr)
		return xasr->delta();
	return 0;
	*/
}

double AMXASScanConfiguration::end(size_t index) const
{
	return regions_->end(index);
	/*
	AMXASRegion *xasr = region(index);
	if(xasr)
		return xasr->end();
	return -1;
	*/
}

AMXASRegion* AMXASScanConfiguration::region(size_t index) const
{
	return NULL;
	/*
	if(index < (size_t)regions_.count())
		return regions_.at(index);
	return NULL;
	*/
}

bool AMXASScanConfiguration::setStart(size_t index, double start)
{
	return regions_->setStart(index, start);
	/*
	if(index < (size_t)regions_.count()){
		AMXASRegion* xasr = regions_.at(index);
		xasr->setStart(start);
		return TRUE;
	}
	return FALSE;
	*/
}

bool AMXASScanConfiguration::setDelta(size_t index, double delta)
{
	return regions_->setDelta(index, delta);
	/*
	if(index < (size_t)regions_.count()){
		AMXASRegion* xasr = regions_.at(index);
		xasr->setDelta(delta);
		return TRUE;
	}
	return FALSE;
	*/
}

bool AMXASScanConfiguration::setEnd(size_t index, double end)
{
	return regions_->setEnd(index, end);
	/*
	if(index < (size_t)regions_.count()){
		AMXASRegion* xasr = regions_.at(index);
		xasr->setEnd(end);
		return TRUE;
	}
	return FALSE;
	*/
}

bool AMXASScanConfiguration::setRegion(size_t index, AMXASRegion *region)
{
	return false;
	/*
	if(region && index < (size_t)regions_.count()){
		AMXASRegion *xasr = regions_.at(index);
		xasr = region;
		return TRUE;
	}
	return FALSE;
	*/
}

bool AMXASScanConfiguration::addRegion(size_t index, AMXASRegion *region)
{
	return false;
	/*
	if(index <= (size_t)regions_.count()){
		regions_.insert(index, region);
		return TRUE;
	}
	return FALSE;
	*/
}

/*
bool AMXASScanConfiguration::addRegion(size_t index, double start, double delta, double end)
{
	AMXASRegion *xasr = new AMXASRegion(SGMBeamline::sgm()->energy(), this);
	xasr->setStart(start);
	xasr->setDelta(delta);
	xasr->setEnd(end);
	return addRegion(index, xasr);
}
*/

bool AMXASScanConfiguration::deleteRegion(size_t index)
{
	return regions_->deleteRegion(index);
	/*
	if(index < (size_t)regions_.count()){
		regions_.removeAt(index);
		return TRUE;
	}
	return FALSE;
	*/
}
