#ifndef AMFASTSCANCONFIGURATION_H
#define AMFASTSCANCONFIGURATION_H

#include "AMScanConfiguration.h"

class AMFastScanConfiguration : public AMScanConfiguration
{
	Q_OBJECT

	Q_PROPERTY(double start READ start WRITE setStart)
	Q_PROPERTY(double end READ end WRITE setEnd)

public:
	AMFastScanConfiguration(QObject *parent = 0);

	double start() const { return start_;}
	double end() const { return end_;}

public slots:
	bool setStart(double start) { start_ = start; setModified(true); return true;}
	bool setEnd(double end) { end_ = end; setModified(true); return true;}

signals:

protected:
	double start_;
	double end_;
};

#endif // AMFASTSCANCONFIGURATION_H
