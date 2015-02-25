#include "CSRDataModel.h"

#include <QTextStream>
#include <QStringList>
#include <QDebug>
#include <QDataStream>

#include <math.h>

#include <QTime>

CSRDataModel::CSRDataModel(QObject *parent) :
	QObject(parent)
{
	mean_ = 0;
	standardDeviation_ = 0;
	dataFile_ = 0;
}

bool CSRDataModel::loadDataFile(const QString &filePath)
{
	QString binaryPath = filePath;
	binaryPath = binaryPath.replace(".txt", ".dat");

	if (!QFile::exists(binaryPath)){

		QFile file(filePath);

		if(!file.open(QIODevice::ReadOnly)) {
			return false;
		}

		QTextStream in(&file);

		QVector<double> timeData = QVector<double>(2e7, 0);
		QVector<double> data = QVector<double>(2e7, 0);

		QTime time;
		time.start();

		for (int i = 0; i < 2e7; i++){

			QStringList data = in.readLine().split("\t");
			timeData[i] = data.at(3).toDouble();
			data[i] = data.at(4).toDouble();
		}

		qDebug() << "Time to load: " << time.restart() << "ms";
		file.close();

		QFile binaryFile(binaryPath);

		if(!binaryFile.open(QIODevice::WriteOnly)) {
			return false;
		}

		QDataStream out(&binaryFile);

		foreach (double val, timeData)
			out << val;

		foreach (double val, data)
			out << val;

		qDebug() << "Time to write binary file: " << time.elapsed() << "ms";

		binaryFile.close();
	}

	dataFile_ = new QFile(binaryPath, this);

	if(!dataFile_->open(QIODevice::ReadOnly)) {
		return false;
	}

	return true;
}

void CSRDataModel::timeData(int start, int end, double *newData) const
{
	QTime time;
	time.start();

	dataFile_->seek(start*sizeof(double));
	int size = end-start+1;
	QVector<double> data = QVector<double>(size, 0);

	QDataStream in(dataFile_);

	for (int i = 0, size = end-start+1; i < size; i++)
		in >> data[i];

	memcpy(newData, data.data(), size*sizeof(double));

	qDebug() << "Time to grab and copy" << size << "time elements:" << time.elapsed() << "ms";
}

void CSRDataModel::data(int start, int end, double *newData) const
{
	QTime time;
	time.start();

	dataFile_->seek((2e7+start)*sizeof(double));
	int size = end-start+1;
	QVector<double> data = QVector<double>(size, 0);

	QDataStream in(dataFile_);

	for (int i = 0, size = end-start+1; i < size; i++)
		in >> data[i];

	memcpy(newData, data.data(), size*sizeof(double));

	qDebug() << "Time to grab and copy" << size << "data elements:" << time.elapsed() << "ms";
}

void CSRDataModel::smoothData(int start, int end, double *newData) const
{
	memcpy(newData, smoothData_.data()+start, (end-start+1)*sizeof(double));
}

void CSRDataModel::compute()
{
	QTime time;
	time.start();

	mean_ = 0;
	double meanSquared = 0;

	QVector<double> allData = QVector<double>(2e7, 0);
	data(0, 2e7-1, allData.data());

	for (int i = 0, size = allData.size(); i < size; i++){

		double value = allData.at(i);
		mean_ += value;
		meanSquared += value*value;
	}

	mean_ /= allData.size();
	meanSquared /= allData.size();

	standardDeviation_ = sqrt(meanSquared - mean_*mean_);
	qDebug() << "Time to compute mean and std dev: " << time.restart() << "ms";

	// 5 point running average
//	smoothData_ = QVector<double>(data_.size(), 0);
//	int smoothingFactor = 2;

//	for (int i = 0, size = smoothData_.size(); i < size; i++){

//		for (int j = -1*smoothingFactor; j <= smoothingFactor; j++){

//			if (((i+j) >= 0) && ((i+j) < size))
//				smoothData_[i] += data_.at(i+j);

//			smoothData_[i] /= smoothingFactor*2;
//		}
//	}

	qDebug() << "Time to smooth data:" << time.restart() << "ms";
}
