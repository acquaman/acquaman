#include "AMRotationalOffset.h"

AMRotationalOffset::AMRotationalOffset(QObject *parent) :
	AMDbObject(parent)
{
	rotationalOffset_ = QVector3D(0,0,0);
}

AMRotationalOffset::AMRotationalOffset(double x, double y, double z, QObject *parent) :
	AMDbObject(parent)
{
	setRotationalOffset(x,y,z);
}

AMRotationalOffset::AMRotationalOffset(QVector3D rotationalOffset, QObject *parent) :
	AMDbObject(parent)
{
	setRotationalOffset(rotationalOffset);
}

QVector3D AMRotationalOffset::rotationalOffset()
{
	return rotationalOffset_;
}

void AMRotationalOffset::setRotationalOffset(QVector3D rotationalOffset)
{
	rotationalOffset_ = rotationalOffset;
}

void AMRotationalOffset::setRotationalOffset(double x, double y, double z)
{
	rotationalOffset_ = QVector3D(x,y,z);
}
