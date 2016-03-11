#include "AMTESTSeriesData.h"

AMTESTSeriesData::AMTESTSeriesData(int dataSize, int updateRate)
	: MPlotAbstractSeriesData()
{
	yAxis_ = AMTESTRingBuffer(dataSize*updateRate);
	updateRate_ = updateRate;
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

void AMTESTSeriesData::addData(const QVector<qreal> &data)
{
	yAxis_.addValues(data);
	int seriesSize = yAxis_.size();

	if (xAxis_.size() != seriesSize){

		xAxis_ = QVector<qreal>(seriesSize, 0);

		for (int i = 0; i < seriesSize; i++)
			xAxis_[i] = -1*(seriesSize-(i+1))/updateRate_;
	}

	emitDataChanged();
}

void AMTESTSeriesData::clear()
{
	xAxis_ = QVector<qreal>();
	yAxis_.clear();
	emitDataChanged();
}
