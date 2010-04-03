#ifndef ACQMAN_XASSCANCONFIGURATION_H
#define ACQMAN_XASSCANCONFIGURATION_H

#include <QObject>
#include "AMScanConfiguration.h"
//#include "beamline/SGMBeamline.h"

class AMXASScanConfiguration : public AMScanConfiguration
{
    Q_OBJECT
public:    
    AMXASScanConfiguration(QObject *parent = 0);
    double start(size_t index) const;
    double delta(size_t index) const;
    double end(size_t index) const;
    AMXASRegion* region(size_t index) const;
    QList<AMXASRegion*> regions() { return regions_;}
	QList<AMXASRegion*>* regionsPtr() { return &regions_;}

public slots:
    bool setStart(size_t index, double start);
    bool setDelta(size_t index, double delta);
    bool setEnd(size_t index, double end);
    bool setRegion(size_t index, AMXASRegion *region);
    virtual bool addRegion(size_t index, AMXASRegion *region);
    virtual bool addRegion(size_t index, double start, double delta, double end) = 0;
    bool deleteRegion(size_t index);

protected:
    QList<AMXASRegion*> regions_;
//    QList<AMControlSet*> groups_;
};

#endif // ACQMAN_XASSCANCONFIGURATION_H
