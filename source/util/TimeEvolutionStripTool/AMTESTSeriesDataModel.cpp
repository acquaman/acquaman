#include "AMTESTSeriesDataModel.h"

AMTESTSeriesDataModel::AMTESTSeriesDataModel(const QString &name, quint32 bufferSize, quint32 updateRate, QObject *parent)
	: AMTESTDataModel(name, parent)
{
	data_ = QVector<qreal>(bufferSize*updateRate, 0);
	dataRingBufferStartIndex_ = 0;
	dataModel_ = new AMTESTSeriesData;
}

AMTESTSeriesDataModel::~AMTESTSeriesDataModel()
{
	delete dataModel_;
}

void AMTESTSeriesDataModel::addData(const QVector<qreal> &newData)
{
	int newSize = newData.size();
	int dataSize = data_.size();

	if (dataRingBufferStartIndex_+newSize < dataSize){

		memcpy(data_.data()+dataRingBufferStartIndex_, newData.constData(), newSize*sizeof(qreal));
		dataRingBufferStartIndex_ += newSize;
	}

	else {

		int distanceToEnd = dataSize-dataRingBufferStartIndex_;
		int distanceFromBeginning = newSize-distanceToEnd;
		memcpy(data_.data()+dataRingBufferStartIndex_, newData.constData(), distanceToEnd*sizeof(qreal));
		memcpy(data_.data(), newData.constData()+distanceToEnd, distanceFromBeginning*sizeof(qreal));
		dataRingBufferStartIndex_ = distanceFromBeginning;
	}
}

