#include "CSRDataModel.h"

#include <QTextStream>
#include <QStringList>
#include <QDataStream>
#include <QDebug>

CSRDataModel::CSRDataModel(QObject *parent) :
	QObject(parent)
{
	dataFile_ = 0;
}

CSRDataModel::~CSRDataModel()
{
	if (dataFile_){

		dataFile_->close();
		dataFile_->deleteLater();
	}

	foreach (CSRFileConfiguration *info, fileConfigurations_)
		info->deleteLater();

	fileConfigurations_.clear();
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

		for (int i = 0; i < 2e7; i++){

			QStringList line = in.readLine().split("\t");
			timeData[i] = line.at(3).toDouble();
			data[i] = line.at(4).toDouble();
		}

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
	dataFile_->seek(start*sizeof(double));
	int size = end-start+1;
	QVector<double> data = QVector<double>(size, 0);

	QDataStream in(dataFile_);

	for (int i = 0, size = end-start+1; i < size; i++)
		in >> data[i];

	memcpy(newData, data.data(), size*sizeof(double));
}

void CSRDataModel::data(int start, int end, double *newData) const
{
	dataFile_->seek((2e7+start)*sizeof(double));
	int size = end-start+1;
	QVector<double> data = QVector<double>(size, 0);

	QDataStream in(dataFile_);

	for (int i = 0, size = end-start+1; i < size; i++)
		in >> data[i];

	memcpy(newData, data.data(), size*sizeof(double));
}

void CSRDataModel::computeIntegral()
{
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
}

void CSRDataModel::addFileConfiguration(CSRFileConfiguration *info)
{
	fileConfigurations_ << info;
}

int CSRDataModel::fileConfigurationCount() const
{
	return fileConfigurations_.size();
}

CSRFileConfiguration * CSRDataModel::fileConfigurationAt(int index) const
{
	return fileConfigurations_.at(index);
}

void CSRDataModel::removeFileConfiguration(CSRFileConfiguration *info)
{
	fileConfigurations_.removeOne(info);
	info->deleteLater();
}
