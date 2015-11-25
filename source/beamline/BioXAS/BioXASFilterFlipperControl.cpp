#include "BioXASFilterFlipperControl.h"

BioXASFilterFlipperControl::BioXASFilterFlipperControl(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{

}

BioXASFilterFlipperControl::~BioXASFilterFlipperControl()
{

}

bool BioXASFilterFlipperControl::setSlideFilter(int index, BioXASFilterFlipperFilter *filter)
{
	if (slideFilterMap_.keys().contains(index))
		removeSlideFilter(index);

	addSlideFilter(index, filter);

	return true;
}

bool BioXASFilterFlipperControl::addSlideFilter(int index, BioXASFilterFlipperFilter *filter)
{
	slideFilterMap_.insert(index, filter);

	if (filter) {
		connect( filter, SIGNAL(elementChanged(const QString*)), this, SIGNAL(filtersChanged()) );
		connect( filter, SIGNAL(thicknessChanged(double)), this, SIGNAL(filtersChanged()) );
	}

	return true;
}

bool BioXASFilterFlipperControl::removeSlideFilter(int index)
{
	bool result = false;

	if (slideFilterMap_.keys().contains(index)) {

		BioXASFilterFlipperFilter *oldFilter = slideFilterMap_.value(index);
		slideFilterMap_.remove(index);

		if (oldFilter)
			disconnect( oldFilter, 0, this, 0 );

		result = true;
	}

	return result;
}
