#include "FCApp.h"
#include "BeamlineShutters.h"


FCApp::FCApp(QWidget *parent) :
    QWidget(parent)
{
    BeamlineShutters *vespersShutters = new BeamlineShutters("VESPERS",
                                                             "PSH1408-B20-01:state",
                                                             "PSH1408-B20-02:state",
                                                             "SSH1408-B20-01:state",
                                                             this);

}
