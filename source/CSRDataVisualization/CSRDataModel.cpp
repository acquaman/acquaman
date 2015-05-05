#include "CSRDataModel.h"

#include <QTextStream>
#include <QStringList>
#include <QDataStream>
#include <QFileInfo>
#include <QDebug>

CSRDataModel::CSRDataModel(QObject *parent) :
	QObject(parent)
{
	dataFile_ = 0;
}

CSRDataModel::CSRDataModel(const CSRDataModel &original)
	: QObject()
{
	dataFile_ = 0;
	fileNames_ = original.fileNames();

	for (int i = 0, size = original.fileConfigurationCount(); i < size; i++)
		fileConfigurations_ << new CSRFileConfiguration(*(original.fileConfigurationAt(i)));
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
	binaryPath.insert(binaryPath.lastIndexOf("/"), "/dataStore");

	if (!QFile::exists(binaryPath)){

		QFile file(filePath);

		if(!file.open(QIODevice::ReadOnly)) {
			return false;
		}

		QTextStream in(&file);

		QVector<double> timeData = QVector<double>(numberOfPoints_, 0);
		QVector<double> data = QVector<double>(numberOfPoints_, 0);

		for (int i = 0; i < numberOfPoints_; i++){

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
	dataFile_->seek((numberOfPoints_+start)*sizeof(double));
	int size = end-start+1;
	QVector<double> data = QVector<double>(size, 0);

	QDataStream in(dataFile_);

	for (int i = 0, size = end-start+1; i < size; i++)
		in >> data[i];

	memcpy(newData, data.data(), size*sizeof(double));
}

void CSRDataModel::computeIntegral()
{
	int revolutions = numberOfRevolutions_;
	int numberPerRevolution = numberOfPoints_/revolutions;

	integral_ = QVector<double>(revolutions, 0);

	for (int i = 0; i < revolutions; i++){

		QVector<double> x = QVector<double>(numberPerRevolution, 0);
		QVector<double> fx = QVector<double>(numberPerRevolution, 0);

		data(i*numberPerRevolution, qMin((i+1)*numberPerRevolution-1, int(numberOfPoints_-1)), fx.data());
		timeData(i*numberPerRevolution, qMin((i+1)*numberPerRevolution-1, int(numberOfPoints_-1)), x.data());

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

void CSRDataModel::setFileNames(const QStringList &newFileNames)
{
	fileNames_ = newFileNames;
}

void CSRDataModel::start()
{
	for (int i = 0, size = fileNames_.size(); i < size; i++){

		setupFileParameters(fileNames_.at(i));
		loadDataFile(fileNames_.at(i));
		computeIntegral();
		writeOutputFile(fileNames_.at(i));
		dataFile_->close();
		dataFile_->deleteLater();
		dataFile_ = 0;
		emit progress(i+1);
	}

	emit finished();
}

void CSRDataModel::writeOutputFile(const QString &filePath)
{
	QString outputPath = filePath;
	outputPath = outputPath.replace(".txt", "_analyzed.txt");
	outputPath.insert(outputPath.lastIndexOf("/"), "/AnalyzedFiles");

	QFile file(outputPath);

	if(!file.open(QIODevice::WriteOnly))
		return;

	QTextStream out(&file);

	out << "Revolutions\tIntegral\n";

	for (int i = 0, size = integral_.size(); i < size; i++)
		out << QString("%1\t%2\n").arg(i+1).arg(integral_.at(i), 0, 'g', 10);

	file.close();
}

void CSRDataModel::setupFileParameters(const QString &filePath)
{
	bool fileAccountedFor = false;
	QFileInfo info(filePath);

	for (int i = 0, size = fileConfigurations_.size(); i < size; i++){

		if (abs(info.size()-fileConfigurations_.at(i)->fileSize()) < int(fileConfigurations_.at(i)->fileSize()*0.05)){

			fileAccountedFor = true;
			fileSize_ = fileConfigurations_.at(i)->fileSize();
			numberOfPoints_ = fileConfigurations_.at(i)->numberOfPoints();
			numberOfRevolutions_ = fileConfigurations_.at(i)->numberOfRevolutions();
		}
	}

	if (!fileAccountedFor){

		fileSize_ = 0;
		numberOfPoints_ = 0;
		numberOfRevolutions_ = 0;
	}
}
