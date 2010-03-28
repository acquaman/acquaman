#ifndef AMREGION_H
#define AMREGION_H

#include <QObject>
#include "beamline/AMControl.h"

class AMRegion: public QObject
{
Q_OBJECT
Q_PROPERTY(double start READ start WRITE setStart)
Q_PROPERTY(double delta READ delta WRITE setDelta)
Q_PROPERTY(double end READ end WRITE setEnd)

public:
AMRegion(QObject *parent = 0);
    virtual double start() const { return start_;}
    virtual double delta() const { return delta_;}
    virtual double end() const { return end_;}
    virtual AMControl* control() const { return ctrl_;}

public slots:
    virtual bool setStart(double start) { start_ = start; return TRUE;}
    virtual bool setDelta(double delta) {
        if(delta == 0)
            return false;
        delta_ = delta;
        return true;
    }
    virtual bool setEnd(double end) { end_ = end; return TRUE;}
    virtual bool setControl(AMControl* ctrl) { ctrl_ = ctrl; return true;}

protected:
    double start_;
    double delta_;
    double end_;
    AMControl *ctrl_;
};

class AMXASRegion : public AMRegion
{
Q_OBJECT

public:
AMXASRegion(AMControl* beamlineEnergy, QObject *parent = 0) : AMRegion(parent) {ctrl_ = beamlineEnergy;}

public slots:
    bool setStart(double start) {
        if(ctrl_->valueOutOfRange(start))
            return FALSE;
        start_ = start;
        return TRUE;
    }
    bool setEnd(double end) {
        if(ctrl_->valueOutOfRange(end))
            return FALSE;
        end_ = end;
        return TRUE;
    }
    bool setControl(AMControl *ctrl){Q_UNUSED(ctrl); return false;}
};

#endif // AMREGION_H
