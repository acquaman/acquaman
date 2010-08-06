#include "AMDetectorInfo.h"

AMDetectorInfo::AMDetectorInfo(const QString& name, const QString& description, QObject *parent) :
		AMDbObject(parent)
{
	setName(name);
	metaData_["description"] = description;
}

/// Load yourself from the database. (returns true on success)
/*! Re-implemented from AMDbObject. */
bool AMDetectorInfo::loadFromDb(AMDatabase* db, int id) {
	bool retval = AMDbObject::loadFromDb(db, id);
	// This was cheating... dimension and rank shouldn't be stored authoritatively in the metaData_. Let's get rid of them.
	metaData_.remove("dimension");
	metaData_.remove("rank");
	return retval;
}

/// Store or update self in the database. (returns true on success)
/*! Re-implemented from AMDbObject::storeToDb(), this version also writes out the dimension and rank, even though they aren't strictly part of the meta-data.
  */
bool AMDetectorInfo::storeToDb(AMDatabase* db) {
	// the base class version is good at saving all the values in the metaData_ hash. Let's just exploit that.
	metaData_["dimension"] = dimension();
	QString rankList;
	foreach(int dim, rank())
		rankList.append("%1,").arg(dim);
	rankList.chop(1);
	metaData_["rank"] = rankList;

	// Call the base class implementation
	bool retval = AMDbObject::storeToDb(db);

	// This was cheating... dimension and rank aren't stored authoritatively in the metaData_. Let's get rid of them.
	metaData_.remove("dimension");
	metaData_.remove("rank");

	return retval;
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
	metaData_["binCount"] = binCount;
	metaData_["axisName"] = axisName;
	metaData_["binNames"] = binNames;
	metaData_["integrationTime"] = double(1.0);
	metaData_["integrationTimeRangeMin"] = double(0.0);
	metaData_["integrationTimeRangeMax"] = double(10.0);
	QStringList tmpList;
	tmpList << "Real" << "Live" << "Peak";
	metaData_["integrationModeList"] = QStringList(tmpList);
	metaData_["integrationMode"] = QString(tmpList.at(0));
}

MCPDetectorInfo::MCPDetectorInfo(const QString& name, const QString& description, QObject *parent) : AMDetectorInfo(name, description, parent)
{
	metaData_["hvSetpoint"] = double(0.0);
	metaData_["hvSetpointRangeMin"] = double(0.0);
	metaData_["hvSetpointRangeMax"] = double(1400.0);
}

PGTDetectorInfo::PGTDetectorInfo(const QString& name, const QString& description, QObject *parent) : AMSpectralOutputDetectorInfo(name, description, 1024, "energy", QStringList(), parent)
{
	metaData_["hvSetpoint"] = double(0.0);
	metaData_["hvSetpointRangeMin"] = double(0.0);
	metaData_["hvSetpointRangeMax"] = double(180.0);
}
