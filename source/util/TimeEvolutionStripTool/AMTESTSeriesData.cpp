#include "AMTESTSeriesData.h"

AMTESTSeriesData::AMTESTSeriesData()
	: MPlotAbstractSeriesData()
{

}

AMTESTSeriesData::~AMTESTSeriesData()
{

}

qreal AMTESTSeriesData::x(unsigned index) const
{
	return xAxis_.at(index);
}

void AMTESTSeriesData::xValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const
{
	memcpy(outputValues, xAxis_.constData()+indexStart, (indexEnd-indexStart+1)*sizeof(qreal));
}

qreal AMTESTSeriesData::y(unsigned index) const
{
	return yAxis_.value(index);
}

void AMTESTSeriesData::yValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const
{
	yAxis_.values(indexStart, indexEnd, outputValues);
}

int AMTESTSeriesData::count() const
{
	return yAxis_.size();
}
