#ifndef AMROTATIONALOFFSET_H
#define AMROTATIONALOFFSET_H

#include <QVector3D>
#include "dataman/database/AMDbObject.h"

class AMRotationalOffset : public AMDbObject
{
	Q_OBJECT
	Q_PROPERTY(QVector3D rotationalOffset READ rotationalOffset WRITE setRotationalOffset)

public:
	AMRotationalOffset(QObject *parent = 0);
	AMRotationalOffset(double x, double y, double z, QObject *parent = 0);
	AMRotationalOffset(QVector3D rotationalOffset, QObject *parent = 0);
	QVector3D rotationalOffset();
	void setRotationalOffset(QVector3D rotationalOffset);
	void setRotationalOffset(double x, double y, double z);

private:
	QVector3D rotationalOffset_;
};

#endif // AMROTATIONALOFFSET_H
