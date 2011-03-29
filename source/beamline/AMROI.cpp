#include "AMROI.h"

AMROI::AMROI(QObject *parent) :
	QObject(parent)
{
}

AMROIInfo AMROI::toInfo()
{
	return AMROIInfo(QString(), 0, 0);
}

void AMROI::fromInfo(const AMROIInfo &info)
{

}

void AMROI::setName(QString name)
{

}

void AMROI::setEnergy(double energy)
{

}

void AMROI::setWidth(double width)
{

}

void AMROI::setLow(int low)
{

}

void AMROI::setHigh(int high)
{

}
