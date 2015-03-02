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
//	QTime time;
//	time.start();

	dataFile_->seek(start*sizeof(double));
	int size = end-start+1;
	QVector<double> data = QVector<double>(size, 0);

	QDataStream in(dataFile_);

	for (int i = 0, size = end-start+1; i < size; i++)
		in >> data[i];

	memcpy(newData, data.data(), size*sizeof(double));

//	qDebug() << "Time to grab and copy" << size << "time elements:" << time.elapsed() << "ms";
}

void CSRDataModel::data(int start, int end, double *newData) const
{
//	QTime time;
//	time.start();

	dataFile_->seek((2e7+start)*sizeof(double));
	int size = end-start+1;
	QVector<double> data = QVector<double>(size, 0);

	QDataStream in(dataFile_);

	for (int i = 0, size = end-start+1; i < size; i++)
		in >> data[i];

	memcpy(newData, data.data(), size*sizeof(double));

//	qDebug() << "Time to grab and copy" << size << "data elements:" << time.elapsed() << "ms";
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
	QList<double> validTimePositions;
	QList<double> validData;

	for (int i = 0; i < size; i++)
		if (fabs(values.at(i)) > fiveStandardDeviations){

			validTimePositions << timeValues.at(i);
			validData << values.at(i);
		}

	if (!validTimePositions.isEmpty()){

		// Get the indices that define peaks.
		QList<int> peakDelimiters;

		for (int i = 0; i < validTimePositions.size()-1; i++)
			if ((validTimePositions.at(i+1)-validTimePositions.at(i)) > 1e-9)
				peakDelimiters << i;

		// If we got in here there is at least one peak, so add the last position.
		peakDelimiters << (validData.size()-1);

		// Get the maximum values per peak.
		maximumValues_ = QList<double>();
		double value = validData.first();
		int peakIndex = 0;
		int pointIndex = 0;
		maximaPoints_ = QList<QPointF>();

		for (int i = 1, validSize = validData.size(); i < validSize; i++){

			if (i == peakDelimiters.at(peakIndex)/* || i == (validSize-1)*/){

				maximumValues_ << validData.at(pointIndex);
				peakIndex++;
				value = validData.at(i);
				maximaPoints_ << QPointF(validTimePositions.at(pointIndex), validData.at(pointIndex));
			}

			else{

				if (value > validData.at(i)){

					value = validData.at(i);
					pointIndex = i;
				}
			}
		}

		// Remove the extra piece we added for max value finding.
		peakDelimiters.removeLast();

		// Find the peak positions.
		peakPositions_ = QList<double>();

		if (peakDelimiters.isEmpty())
			peakPositions_ << validTimePositions.at(int(validTimePositions.size()/2));

		else{

			peakDelimiters.prepend(0);

			for (int i = 0, size = peakDelimiters.size()-1; i < size; i++)
				peakPositions_ << validTimePositions.at(peakDelimiters.at(i) + int((peakDelimiters.at(i+1)-peakDelimiters.at(i))/2));

			peakPositions_ << validTimePositions.at(peakDelimiters.at(peakDelimiters.size()-1) + int((validTimePositions.size()-peakDelimiters.at(peakDelimiters.size()-1))/2));
		}
	}
}

void CSRDataModel::compute()
{
	QTime time;
	time.start();

	int size = 877;
	int numberPerRevolution = 2e7/size;

	mainToWakeFieldDifferences_ = QVector<double>(size, 0);
	mainToMainDifferences_ = QVector<double>(size-1, 0);
	mainPeakMaximum_ = QVector<double>(size, 0);
	wakefieldPeakMaximum_ = QVector<double>(size, 0);

	QList<double> mainPeakPositions;

	for (int i = 0; i < size; i++){

		findPeaks(i*numberPerRevolution, qMin((i+1)*numberPerRevolution-1, int(2e7-1)));
		mainToWakeFieldDifferences_[i] = peakPositions_.at(1)-peakPositions_.at(0);
		mainPeakPositions << peakPositions_.at(0);
		mainPeakMaximum_[i] = maximumValues_.first();
		wakefieldPeakMaximum_[i] = maximumValues_.last();
	}

	for (int i = 0; i < size-1; i++)
		mainToMainDifferences_[i] = mainPeakPositions.at(i+1)-mainPeakPositions.at(i);

	qDebug() << "Time to find all the peak differences:" << time.elapsed() << "ms";
}

void CSRDataModel::computeIntegral()
{
	QTime time;
	time.start();

	int size = 877;
	int numberPerRevolution = 2e7/size;

	integral_ = QVector<double>(size, 0);

	for (int i = 0; i < size; i++){

		QVector<double> x = QVector<double>(numberPerRevolution, 0);
		QVector<double> fx = QVector<double>(numberPerRevolution, 0);

		data(i*numberPerRevolution, qMin((i+1)*numberPerRevolution-1, int(2e7-1)), fx.data());
		timeData(i*numberPerRevolution, qMin((i+1)*numberPerRevolution-1, int(2e7-1)), x.data());

		double partialSum = fx.first()+fx.last();

		for (int j = 1; j < numberPerRevolution-1; j++){

			partialSum += 2*fx.at(j);
		}

		integral_[i] = partialSum*(x.last()-x.first())/(2*(numberPerRevolution-1));
	}

	qDebug() << "Time to find the integral: " << time.elapsed() << "ms";
}
