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


#ifndef BIOXASSHUTTERTOOLCONTROLLER_H
#define BIOXASSHUTTERTOOLCONTROLLER_H

#include <QObject>

#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "source/actions3/AMAction3.h"
#include "actions3/AMActionSupport.h"
#include "source/actions3/AMListAction3.h"

class BioXASShutterToolController : public QObject
{
    Q_OBJECT

public:
    enum ShutterState { Closed = 0,
                        Open
                      };

    enum ControlState { Disabled = 0,
                        Enabled
                      };

	explicit BioXASShutterToolController(QObject *parent = 0);

signals:
    void aodShutterControlConnected(bool isConnected);
    void preInjectionControlsDisabled();
    void postInjectionControlsEnabled();
    void shuttersConnected(bool areConnected);
    void shuttersOpen();
    void shuttersClosed();
    void shuttersBetween();
    void errorReported();

public:
    bool aodShutterControlConnected();
    bool shuttersConnected();
    bool isOpen();
    bool isClosed();
    bool isBetween();

public slots:
    void openShutters();
    void closeShutters();
    void setAutomaticShuttersOpen(bool enabled);

protected slots:
    void onAODShutterControlConnected(bool isConnected);
    void onAODShutterControlValueChanged(double controlPVState);

    void onSafetyShutterConnected(bool isConnected);
    void onSafetyShutterStateChanged(int newState);

    void onPhotonShutterConnected(bool isConnected);
    void onPhotonShutterStateChanged(int newState);

    void onOpenActionFinished();
    void onCloseActionFinished();
    void onErrorReported(const QString &errorInfo);

private:
    void createComponents();
    void makeConnections();
    void connectedCheck();
    void shutterStateCheck();
    AMAction3* createOpenShuttersAction();
    AMAction3* createCloseShuttersAction();

private:
    AMReadOnlyPVControl* aodShutterControl_;
    CLSBiStateControl* safetyShutter_;
    CLSBiStateControl* photonShutter_;
    bool automaticShuttersOpen_;
    AMAction3* openAction_;
    AMAction3* closeAction_;

};

#endif // BIOXASSHUTTERTOOLCONTROLLER_H
