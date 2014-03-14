#ifndef MODEL_H
#define MODEL_H

#include <QObject>

#include "beamline/AMPVControl.h"
#include "beamline/CLS/CLSBiStateControl.h"
#include "source/actions3/AMAction3.h"
#include "source/actions3/actions/AMControlMoveAction3.h"
#include "source/actions3/AMListAction3.h"

class Model : public QObject
{
    Q_OBJECT

public:
    enum ShutterState { Closed = 0,
                        Open
                      };

    enum ControlState { Disabled = 0,
                        Enabled
                      };

    explicit Model(QObject *parent = 0);

signals:
    void connected(bool isConnected);
    void shuttersOpen();
    void shuttersClosed();
    void shuttersBetween();
    void errorReported();

public:
    bool isConnected();
    bool isOpen();
    bool isClosed();
    bool isBetween();

public slots:
    void openShutters();
    void closeShutters();
    void setAutomaticControl(bool controlOn);

protected slots:
    void onShutterControlEnabledConnected(bool isConnected);
    void onShutterControlEnabledValueChanged(double controlPVState);

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
    AMReadOnlyPVControl* controlEnabledStatus_;
    CLSBiStateControl* safetyShutter_;
    CLSBiStateControl* photonShutter_;
    bool automaticControlOn_;
    AMAction3* openAction_;
    AMAction3* closeAction_;

};

#endif // MODEL_H
