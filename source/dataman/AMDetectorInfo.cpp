#include "AMDetectorInfo.h"

AMDetectorInfo::AMDetectorInfo(const QString& name, const QString& description, QObject *parent) :
		AMDbObject(parent)
{
	setName(name);
	description_ = description;
}



/*
#include <QImage>
#include <QBuffer>

AMDbThumbnail AMDetectorInfo::thumbnail(int index) const {

	Q_UNUSED(index)

	QImage image(":/multimedia-volumne-control.png");

	QImage thumbnailImage = image.scaled(QSize(240,180), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	QBuffer boutput;
	boutput.open(QIODevice::WriteOnly);
	thumbnailImage.save(&boutput, "PNG");
	boutput.close();
	return AMDbThumbnail(name(), description(), AMDbThumbnail::PNGType, boutput.buffer());

}*/


AMSpectralOutputDetectorInfo::AMSpectralOutputDetectorInfo(const QString& name, const QString& description, int binCount, QString axisName, QStringList binNames, QObject *parent) :
		AMDetectorInfo(name, description, parent)
{
	binCount_ = binCount;
	axisName_ = axisName;
	binNames_ = binNames;
	integrationTime_ = double(1.0);
	integrationTimeRangeMin_ = double(0.0);
	integrationTimeRangeMax_ = double(10.0);
	integrationModeList_ << "Real" << "Live" << "Peak";
	integrationMode_ = integrationModeList_.at(0);
}

MCPDetectorInfo::MCPDetectorInfo(const QString& name, const QString& description, QObject *parent) : AMDetectorInfo(name, description, parent)
{
	hvSetpoint_ = double(0.0);
	hvSetpointRangeMin_ = double(0.0);
	hvSetpointRangeMax_ = double(1400.0);
}

PGTDetectorInfo::PGTDetectorInfo(const QString& name, const QString& description, QObject *parent) : AMSpectralOutputDetectorInfo(name, description, 1024, "energy", QStringList(), parent)
{
	hvSetpoint_ = double(0.0);
	hvSetpointRangeMin_ = double(0.0);
	hvSetpointRangeMax_ = double(180.0);
}
