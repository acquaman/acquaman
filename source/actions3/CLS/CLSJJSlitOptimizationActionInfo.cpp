#include "CLSJJSlitOptimizationActionInfo.h"

CLSJJSlitOptimizationActionInfo::CLSJJSlitOptimizationActionInfo(CLSJJSlits::Direction::Option direction, CLSJJSlits::Property::Option property, double initialVerticalGap, double initialVerticalCenter, double initialHorizontalGap, double initialHorizontalCenter, CLSJJSlitScanConfiguration *scanConfiguration, QObject *parent) :
	AMListActionInfo3(QString(), QString(), QString(), parent)
{
	direction_ = direction;
	property_ = property;

	initialVerticalGap_ = initialVerticalGap;
	initialVerticalCenter_ = initialVerticalCenter;
	initialHorizontalGap_ = initialHorizontalGap;
	initialHorizontalCenter_ = initialHorizontalCenter;

	scanConfiguration_ = scanConfiguration;

	setShortDescription(typeDescription());
	setLongDescription(typeDescription());
}

CLSJJSlitOptimizationActionInfo::CLSJJSlitOptimizationActionInfo(const CLSJJSlitOptimizationActionInfo &original) :
	AMListActionInfo3(original)
{
	direction_ = original.direction();
	property_ = original.property();

	initialVerticalGap_ = original.initialVerticalGap();
	initialVerticalCenter_ = original.initialVerticalCenter();
	initialHorizontalGap_ = original.initialHorizontalGap();
	initialHorizontalCenter_ = original.initialHorizontalCenter();

	scanConfiguration_ = original.scanConfiguration();
}

CLSJJSlitOptimizationActionInfo::~CLSJJSlitOptimizationActionInfo()
{

}

AMActionInfo3* CLSJJSlitOptimizationActionInfo::createCopy() const
{
	AMActionInfo3 *info = new CLSJJSlitOptimizationActionInfo(*this);
	info->dissociateFromDb(true);
	return info;
}
