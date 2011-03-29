#include "AMROIInfo.h"

AMROIInfo::AMROIInfo(const QString &name, double energy, double width, QObject *parent)
	: AMDbObject(parent)
{
	setName(name);
	energy_ = energy;
	width_ = width;
}

AMROIInfo::AMROIInfo(AMDatabase *db, int id)
	: AMDbObject()
{
	setName("Invalid ROI");
	energy_ = 0;
	width_ = 0;

	loadFromDb(db, id);
}
