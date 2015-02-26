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

CSRDataModel::~CSRDataModel()
{
	dataFile_->close();
	dataFile_->deleteLater();
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

			QStringList line = in.readLine().split("\t");
			timeData[i] = line.at(3).toDouble();
			data[i] = line.at(4).toDouble();
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

	dataFile_ = new QFile(binaryPath);

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

void CSRDataModel::computeMeanAndStandardDeviation()
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
}

void CSRDataModel::findPeaks(int start, int end)
{
	double fiveStandardDeviations = standardDeviation_*5;

	int size = end-start+1;
	QVector<double> values = QVector<double>(size, 0);
	QVector<double> timeValues = QVector<double>(size, 0);

	data(start, end, values.data());
	timeData(start, end, timeValues.data());
	QList<double> validPositions;

	for (int i = 0; i < size; i++)
		if (fabs(values.at(i)) > fiveStandardDeviations)
			validPositions << timeValues.at(i);

	if (!validPositions.isEmpty()){

		QList<int> peakDelimiters;

		for (int i = 0; i < validPositions.size()-1; i++)
			if ((validPositions.at(i+1)-validPositions.at(i)) > 1e-9)
				peakDelimiters << i;

		peakPositions_ = QList<double>();

		if (peakDelimiters.isEmpty())
			peakPositions_ << validPositions.at(int(validPositions.size()/2));

		else{

			peakDelimiters.prepend(0);

			for (int i = 0, size = peakDelimiters.size()-1; i < size; i++)
				peakPositions_ << validPositions.at(peakDelimiters.at(i) + int((peakDelimiters.at(i+1)-peakDelimiters.at(i))/2));

			peakPositions_ << validPositions.at(peakDelimiters.at(peakDelimiters.size()-1) + int((validPositions.size()-peakDelimiters.at(peakDelimiters.size()-1))/2));
		}
	}
}
