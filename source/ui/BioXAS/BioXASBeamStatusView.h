#ifndef BIOXASBEAMSTATUSVIEW_H
#define BIOXASBEAMSTATUSVIEW_H

#include <QWidget>

#include "ui/beamline/AMExtendedControlEditor.h"

class BioXASBeamStatusView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
    explicit BioXASBeamStatusView(QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASBeamStatusView();

signals:

public slots:

protected:
//	/// The editor for the front-end beam status.
//	AMExtendedControlEditor *frontEndStatus_;
//	/// The label holding the valves status LED.
//	QLabel *valvesStatus_;
//	/// The label holding the safety shutter status LED.
//	QLabel *safetyShutterStatus_;
//	/// The label holding the downstream photon shutter status LED.
//	QLabel *downstreamPhotonShutterStatus_;
//	/// The label holding the upstream photon shutter status LED.
//	QLabel *upstreamPhotonShutterStatus_;
};

#endif // BIOXASBEAMSTATUSVIEW_H
