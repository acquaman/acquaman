#include "AMTESTRingBuffer.h"

AMTESTRingBuffer::AMTESTRingBuffer()
{
	data_ = QVector<qreal>(0);
	zeroPosition_ = 0;
	size_ = 0;
}

AMTESTRingBuffer::AMTESTRingBuffer(int ringBufferSize)
{
	data_ = QVector<qreal>(ringBufferSize, 0);
	zeroPosition_ = 0;
	size_ = 0;
}

AMTESTRingBuffer::~AMTESTRingBuffer()
{

}

qreal AMTESTRingBuffer::value(int index) const
{
	return data_.at(ringBufferIndex(index));
}

void AMTESTRingBuffer::values(int startIndex, int endIndex, qreal *outputValues) const
{
	if (size_ < data_.size()){

		memcpy(outputValues, data_.constData()+startIndex, (endIndex-startIndex+1)*sizeof(qreal));
	}

	else {
		int modifiedStartIndex = ringBufferIndex(startIndex);
		int modifiedEndIndex = ringBufferIndex(endIndex);

		if (modifiedStartIndex < modifiedEndIndex)
			memcpy(outputValues, data_.constData()+modifiedStartIndex, (modifiedEndIndex-modifiedStartIndex+1)*sizeof(qreal));

		else {

			int sizeToEnd = size_-modifiedStartIndex;
			memcpy(outputValues, data_.constData()+modifiedStartIndex, sizeToEnd*sizeof(qreal));
			memcpy(outputValues+sizeToEnd, data_.constData(), (modifiedEndIndex+1)*sizeof(qreal));
		}
	}
}

void AMTESTRingBuffer::addValue(qreal value)
{
	if (size_ < data_.size()){

		data_[size_] = value;
		size_++;
	}

	else {

		data_[zeroPosition_] = value;
		zeroPosition_ = (zeroPosition_+1)%size_;
	}

}

void AMTESTRingBuffer::addValues(const QVector<qreal> &data)
{
	if (size_ < data_.size()){

		if (size_+data.size() <= data_.size()){

			memcpy(data_.data()+size_, data.constData(), data.size()*sizeof(qreal));
			size_ += data.size();
		}

		else {

			int sizeToEnd = data_.size()-size_;
			int sizeFromBeginning = data.size()-sizeToEnd;
			memcpy(data_.data()+size_, data.constData(), sizeToEnd*sizeof(qreal));
			memcpy(data_.data(), data.constData()+sizeToEnd, sizeFromBeginning*sizeof(qreal));
			zeroPosition_ = sizeFromBeginning;
			size_ = data_.size();
		}
	}

	else {

		if (zeroPosition_+data.size() <= data_.size()){

			memcpy(data_.data()+zeroPosition_, data.constData(), data.size()*sizeof(qreal));
			zeroPosition_ += data.size();
		}

		else {
			int sizeToEnd = data_.size()-zeroPosition_;
			int sizeFromBeginning = data.size()-sizeToEnd;
			memcpy(data_.data()+zeroPosition_, data.constData(), sizeToEnd*sizeof(qreal));
			memcpy(data_.data(), data.constData()+sizeToEnd, sizeFromBeginning*sizeof(qreal));
			zeroPosition_ = sizeFromBeginning;
		}
	}
}

int AMTESTRingBuffer::size() const
{
	return size_;
}

void AMTESTRingBuffer::setRingBufferSize(int bufferSize)
{
	QVector<qreal> tempData = QVector<qreal>(size_);
	values(0, size_-1, tempData.data());
	clear();
	data_ = QVector<qreal>(bufferSize);
	int copySize = (tempData.size() < bufferSize) ? tempData.size() : bufferSize;
	memcpy(data_.data(), tempData.constData(), copySize*sizeof(qreal));
}

void AMTESTRingBuffer::clear()
{
	zeroPosition_ = 0;
	size_ = 0;
}

int AMTESTRingBuffer::ringBufferIndex(int index) const
{
	if (zeroPosition_+index < size_)
		return zeroPosition_+index;

	else
		return index-(size_-zeroPosition_);
}

