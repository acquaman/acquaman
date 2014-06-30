/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").
Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "REIXSSampleMoveActionInfo.h"
#include "dataman/AMSamplePlatePre2013.h"

REIXSSampleMoveActionInfo::REIXSSampleMoveActionInfo(QObject *parent) :
	AMActionInfo("REIXS Sample Move", "REIXS Sample Move", ":/32x32/gnome-display-properties.png", parent)
{
	// safe nominal measurement positions
	x_ = 0;
	y_ = 0;
	z_ = 0;
	theta_ = 45;

	samplePlateId_ = 0;
	sampleIndex_ = 0;

	updateDescriptions();
}

REIXSSampleMoveActionInfo::REIXSSampleMoveActionInfo(double x, double y, double z, double theta, QObject* parent) :
	AMActionInfo("REIXS Sample Move", "REIXS Sample Move", ":/32x32/gnome-display-properties.png", parent)
{
	x_ = x;
	y_ = y;
	z_ = z;
	theta_ = theta;

	samplePlateId_ = 0;
	sampleIndex_ = 0;

	updateDescriptions();
}


REIXSSampleMoveActionInfo::REIXSSampleMoveActionInfo(const AMControlInfoList &targetPosition, QObject *parent) :
	AMActionInfo("REIXS Sample Move", "REIXS Sample Move", ":/32x32/gnome-display-properties.png", parent)
{
	x_ = 0;
	y_ = 0;
	z_ = 0;
	theta_ = 45;

	samplePlateId_ = 0;
	sampleIndex_ = 0;

	int index;
	if((index = targetPosition.indexOf("sampleX")) != -1)
		x_ = targetPosition.at(index).value();
	if((index = targetPosition.indexOf("sampleY")) != -1)
		y_ = targetPosition.at(index).value();
	if((index = targetPosition.indexOf("sampleZ")) != -1)
		z_ = targetPosition.at(index).value();
	if((index = targetPosition.indexOf("sampleTheta")) != -1)
		theta_ = targetPosition.at(index).value();

	updateDescriptions();
}

REIXSSampleMoveActionInfo::REIXSSampleMoveActionInfo(int samplePlateId, int sampleIndex, QObject* parent) :
	AMActionInfo("REIXS Sample Move", "REIXSSampleMove", ":/32x32/gnome-display-properties.png", parent) {

	x_ = 0;
	y_ = 0;
	z_ = 0;
	theta_ = 45;

	samplePlateId_ = samplePlateId;
	sampleIndex_ = sampleIndex;

	updateDescriptions();
}

REIXSSampleMoveActionInfo::REIXSSampleMoveActionInfo(const REIXSSampleMoveActionInfo &other) :
	AMActionInfo(other)
{
	x_ = other.x_;
	y_ = other.y_;
	z_ = other.z_;
	theta_ = other.theta_;

	samplePlateId_ = other.samplePlateId_;
	sampleIndex_ = other.sampleIndex_;
}

void REIXSSampleMoveActionInfo::updateDescriptions()
{
	// Working off a stored sample plate?
	if(samplePlateId_ > 0) {
		AMSamplePlatePre2013 plate;
		if(!plate.loadFromDb(AMDatabase::database("user"), samplePlateId_) || sampleIndex_ >= plate.count() || sampleIndex_ < 0) {
			setShortDescription("Sample Move to [Invalid Sample Plate]");
			setLongDescription("Sample Move to [Invalid Sample Plate]");
			return;
		}
//		AMControlInfoList positions = plate.at(sampleIndex_).position();
		QString sampleName = AMSamplePre2013::sampleNameForId(plate.database(), plate.at(sampleIndex_).sampleId());
		QString shortDesc = QString("Move to sample '%1' on plate '%2'.").arg(sampleName).arg(plate.name());
		// Problem with appending the positions: currently we don't update the description when that sample is re-marked, so this position string could be misleading in that situation. Just leave it out.
//		QStringList posString;
//		for(int i=0, cc=positions.count(); i<cc; ++i) {
//			const AMControlInfo& pos = positions.at(i);
//			posString << QString("%1: %2 %3  ").arg(pos.contextKnownDescription()).arg(pos.value()).arg(pos.units());
//		}
//		shortDesc.append(" (").append(posString.join(QString())).append(")");

		setShortDescription(shortDesc);
		setLongDescription(shortDesc);
	}

	// working off a fixed position.
	else {

		QString shortDesc = QString("Sample Move: X: %1 mm  Y: %2 mm  Z: %3 mm  Theta: %4 deg")
				.arg(x_)
				.arg(y_)
				.arg(z_)
				.arg(theta_);

		setShortDescription(shortDesc);
		setLongDescription(shortDesc);
	}
}

