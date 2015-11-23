#include "AMTESTSeriesData.h"

AMTESTSeriesData::AMTESTSeriesData()
	: MPlotAbstractSeriesData()
{
	dataStartIndex_ = 0;
}

AMTESTSeriesData::~AMTESTSeriesData()
{

}

qreal AMTESTSeriesData::x(unsigned index) const
{
	return xAxis_.at(dataStartIndex_+index);
}

void AMTESTSeriesData::xValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const
{
	int totalSizeRequested = indexEnd-indexStart+1;
	int totalSize = count();

	if (dataStartIndex_+totalSizeRequested < totalSize){

		memcpy(outputValues, xAxis_.constData()+dataStartIndex_+indexStart, totalSizeRequested*sizeof(qreal));
	}

	else {

		int sizeToEnd = totalSize-dataStartIndex_;
		int sizeFromBeginning = totalSizeRequested-sizeToEnd;
		memcpy(outputValues, xAxis_.constData()+dataStartIndex_+indexStart, sizeToEnd*sizeof(qreal));
		memcpy(outputValues+sizeToEnd, xAxis_.constData(), sizeFromBeginning*sizeof(qreal));
	}
}

qreal AMTESTSeriesData::y(unsigned index) const
{
	return yAxis_.at(dataStartIndex_+index);
}

void AMTESTSeriesData::yValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const
{
	int totalSizeRequested = indexEnd-indexStart+1;
	int totalSize = count();

	if (dataStartIndex_+totalSizeRequested < totalSize){

		memcpy(outputValues, xAxis_.constData()+dataStartIndex_+indexStart, totalSizeRequested*sizeof(qreal));
	}

	else {

		int sizeToEnd = totalSize-dataStartIndex_;
		int sizeFromBeginning = totalSizeRequested-sizeToEnd;
		memcpy(outputValues, xAxis_.constData()+dataStartIndex_+indexStart, sizeToEnd*sizeof(qreal));
		memcpy(outputValues+sizeToEnd, xAxis_.constData(), sizeFromBeginning*sizeof(qreal));
	}
}

int AMTESTSeriesData::count() const
{
	return xAxis_.size();
}
