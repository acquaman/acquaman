#include "AMTESTImageData.h"

AMTESTImageData::AMTESTImageData(int dataSize, int updateRate, int axisSize)
	: MPlotAbstractImageData()
{
	range_ = MPlotRange();
	yAxis_ = QVector<qreal>(axisSize);

	for (int i = 0; i < axisSize; i++)
		yAxis_[i] = i;

	zAxis_ = AMTESTRingBuffer(dataSize*updateRate*axisSize);
	updateRate_ = updateRate;
}

AMTESTImageData::~AMTESTImageData()
{

}

qreal AMTESTImageData::x(int index) const
{
	return xAxis_.at(index);
}

qreal AMTESTImageData::y(int index) const
{
	return yAxis_.at(index);
}

qreal AMTESTImageData::z(int xIndex, int yIndex) const
{
	return zAxis_.value(xIndex*yAxis_.size()+yIndex);
}

void AMTESTImageData::zValues(int xStart, int yStart, int xEnd, int yEnd, qreal *outputValues) const
{
	int ringBufferStart = xStart*yAxis_.size()+yStart;
	int ringBufferEnd = xEnd*yAxis_.size()+yEnd;
	zAxis_.values(ringBufferStart, ringBufferEnd, outputValues);
}

QPoint AMTESTImageData::count() const
{
	return QPoint(xAxis_.size(), yAxis_.size());
}

QRectF AMTESTImageData::boundingRect() const
{
	return boundingRect_;
}

void AMTESTImageData::addData(const QVector<qreal> &data)
{
	double minimum = data.first();
	double maximum = minimum;

	for (int i = 1, size = data.size(); i < size; i++){

		double value = data.at(i);

		if (value < minimum)
			minimum = value;

		if (value > maximum)
			maximum = value;
	}

	if (range_.isNull())
		range_ = MPlotRange(minimum, maximum);

	else{
		range_.setX(qMin(range_.x(), minimum));
		range_.setY(qMax(range_.y(), maximum));
	}

	zAxis_.addValues(data);
	int imageSize = xAxis_.size()*yAxis_.size();

	if (zAxis_.size() != imageSize){

		int xSize = zAxis_.size()/yAxis_.size();
		xAxis_ = QVector<qreal>(xSize, 0);

		for (int i = 0; i < xSize; i++)
			xAxis_[i] = -1*(xSize-(i+1))/updateRate_;

		if(xSize == 0)
			boundingRect_ = QRectF();

		else {

			double minimumX = xAxis_.first();
			double maximumX = xAxis_.last();
			double minimumY = yAxis_.first();
			double maximumY = yAxis_.last();

			boundingRect_ = QRectF(minimumX, minimumY, maximumX-minimumX, maximumY-minimumY);
			emitBoundsChanged();
		}
	}

	emitDataChanged();
}

void AMTESTImageData::clear()
{
	xAxis_ = QVector<qreal>();
	zAxis_.clear();
	emitDataChanged();
}

