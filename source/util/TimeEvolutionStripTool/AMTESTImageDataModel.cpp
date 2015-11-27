#include "AMTESTImageDataModel.h"

AMTESTImageDataModel::AMTESTImageDataModel(const QString &name, quint32 bufferSize, quint32 updateRate, quint32 axisSize, QObject *parent)
	: AMTESTDataModel(name, parent)
{
	data_ = AMTESTRingBuffer(bufferSize*updateRate*axisSize);
	dataModel_ = new AMTESTImageData(bufferSize, updateRate, axisSize);
}

AMTESTImageDataModel::~AMTESTImageDataModel()
{
	delete dataModel_;
}

void AMTESTImageDataModel::addData(const QVector<qreal> &newData)
{
	data_.addValues(newData);
	dataModel_->addData(newData);
}

void AMTESTImageDataModel::clear()
{
	data_.clear();
	dataModel_->clear();
}

