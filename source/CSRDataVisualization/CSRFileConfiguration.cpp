#include "CSRFileConfiguration.h"

CSRFileConfiguration::CSRFileConfiguration(int fileSize, int numberOfPoints, int numberOfRevolutions, QObject *parent)
	: QObject(parent)
{
	fileSize_ = fileSize;
	numberOfPoints_ = numberOfPoints;
	numberOfRevolutions_ = numberOfRevolutions;
}

CSRFileConfiguration::CSRFileConfiguration(const CSRFileConfiguration &original)
	: QObject()
{
	fileSize_ = original.fileSize();
	numberOfPoints_ = original.numberOfPoints();
	numberOfRevolutions_ = original.numberOfRevolutions();
}

bool CSRFileConfiguration::operator ==(const CSRFileConfiguration &other)
{
	return (fileSize_ == other.fileSize())
			&& (numberOfPoints_ == other.numberOfPoints())
			&& (numberOfRevolutions_ == other.numberOfRevolutions());
}

bool CSRFileConfiguration::operator !=(const CSRFileConfiguration &other)
{
	return !(this->operator ==(other));
}

CSRFileConfiguration &CSRFileConfiguration::operator =(const CSRFileConfiguration &other)
{
	if (this != &other){

		fileSize_ = other.fileSize();
		numberOfPoints_ = other.numberOfPoints();
		numberOfRevolutions_ = other.numberOfRevolutions();
	}

	return *this;
}

void CSRFileConfiguration::setFileSize(int newSize)
{
	if (fileSize_ != newSize)
		fileSize_ = newSize;
}

void CSRFileConfiguration::setNumberOfPoints(int newNumberOfPoints)
{
	if (numberOfPoints_ != newNumberOfPoints)
		numberOfPoints_ = newNumberOfPoints;
}

void CSRFileConfiguration::setNumberOfRevolutions(int newNumberOfRevolutions)
{
	if (numberOfRevolutions_ != newNumberOfRevolutions)
		numberOfRevolutions_ = newNumberOfRevolutions;
}
