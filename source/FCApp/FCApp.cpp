#include "FCApp.h"
#include "BeamlineShutters.h"


FCApp::FCApp(QWidget *parent) :
    QWidget(parent)
{
    BeamlineShutters *vespersShutters = new BeamlineShutters("VESPERS",
                                                             "pv123",
                                                             "pv123",
                                                             "pv123",
                                                             this);

}
