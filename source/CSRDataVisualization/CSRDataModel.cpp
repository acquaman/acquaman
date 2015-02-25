#include "CSRDataModel.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>

#include <math.h>

#include <QTime>

CSRDataModel::CSRDataModel(QObject *parent) :
	QObject(parent)
{
	mean_ = 0;
	standardDeviation_ = 0;
}

bool CSRDataModel::loadDataFile(const QString &filePath)
{
	QFile file(filePath);
	if(!file.open(QIODevice::ReadOnly)) {
		return false;
	}

	QTextStream in(&file);

	timeData_ = QVector<double>(2e7, 0);
	data_ = QVector<double>(2e7, 0);

	QTime time;
	time.start();
	// There are two types of raw data sources.  The spectra of rank 1 and the dead time of rank 0.
	for (int i = 0; i < 2e7; i++){

		QStringList data = in.readLine().split("\t");
		timeData_[i] = data.at(3).toDouble();
		data_[i] = data.at(4).toDouble();
	}
	qDebug() << "Time to load: " << time.elapsed() << "ms";
	file.close();

	return true;
}

void CSRDataModel::timeData(double *newData) const
{
	memcpy(newData, timeData_.data(), 2e7*sizeof(double));
}

void CSRDataModel::data(double *newData) const
{
	memcpy(newData, data_.data(), 2e7*sizeof(double));
}

void CSRDataModel::smoothData(double *newData) const
{
	memcpy(newData, smoothData_.data(), 2e7*sizeof(double));
}

void CSRDataModel::compute()
{
	QTime time;
	time.start();

	mean_ = 0;
	double meanSquared = 0;

	for (int i = 0, size = data_.size(); i < size; i++){

		double value = data_.at(i);
		mean_ += value;
		meanSquared += value*value;
	}

	mean_ /= data_.size();
	meanSquared /= data_.size();

	standardDeviation_ = sqrt(meanSquared - mean_*mean_);
	qDebug() << "Time to compute mean and std dev: " << time.restart() << "ms";

	// 5 point running average
	smoothData_ = QVector<double>(data_.size(), 0);
	int smoothingFactor = 2;

	for (int i = 0, size = smoothData_.size(); i < size; i++){

		for (int j = -1*smoothingFactor; j <= smoothingFactor; j++){

			if (((i+j) >= 0) && ((i+j) < size))
				smoothData_[i] += data_.at(i+j);

			smoothData_[i] /= smoothingFactor*2;
		}
	}

	qDebug() << "Time to smooth data:" << time.restart() << "ms";
}
