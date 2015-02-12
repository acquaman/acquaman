#ifndef BIOXASMAINMONOCHROMATORCRYSTALCHANGECONTROL_H
#define BIOXASMAINMONOCHROMATORCRYSTALCHANGECONTROL_H

#include <QObject>

#include "BioXASMainMonochromator.h"

class BioXASMainMonochromatorCrystalChangeControl : public QObject
{
    Q_OBJECT

public:
    /// Enum marking possible different control states.
    enum State { None = 0, Initialized, Running, CompleteSuccess, CompleteFail };
    /// Constructor.
    explicit BioXASMainMonochromatorCrystalChangeControl(BioXASMainMonochromator *mono, QObject *parent = 0);
    /// Destructor.
    virtual ~BioXASMainMonochromatorCrystalChangeControl();
    /// Returns the monochromator being controlled.
    BioXASMainMonochromator* mono() const { return mono_; }
    /// Returns the current state of the control.
    State state() const { return state_; }
    QString stateToString(BioXASMainMonochromatorCrystalChangeControl::State state) const;

signals:
    /// Notifier that the mono being controlled has changed.
    void monoChanged(BioXASMainMonochromator *newMono);
    /// Notifier that the control's current state has changed.
    void stateChanged(BioXASMainMonochromatorCrystalChangeControl::State newState);
    /// Notifier that the currently running action has changed, and so has the corresponding description and user information.
    void currentActionChanged(const QString &newDescription, const QString &newInformation);
    /// Notifier that the action execution progress has changed. Contains info suitable for a progress bar.
    void progressChanged(double value, double valueMax);

public slots:
    /// Sets the monochromator.
    void setMono(BioXASMainMonochromator *newMono);
    /// Starts the crystal change, if current mono is valid and there isn't a crytal change running already.
    void startCrystalChange();
    /// Resets the state of the control. State goes to either None or Initialized, as conditions allow. The idea is to reset the state before and/or after a crystal change, to make sure we don't dwell in a CompleteSuccess or CompleteFail state from a previous change.
    void reset();

protected slots:
    /// Sets the control's current state.
    void setState(State newState);
    /// Handles updating the state when the mono's connection state has changed.
    void onMonoConnectedChanged();
    /// Handles updating the state when the crystal change actions have started.
    void onCrystalChangeActionsStarted();
    /// Handles when the currently executing subaction has changed.
    void onCrystalChangeSubActionChanged(int actionIndex);
    /// Handles updating the state when the crystal change actions have succeeded.
    void onCrystalChangeActionsSucceeded();
    /// Handles updating the state when the crystal change actions report themselves cancelled/failed/destroyed.
    void onCrystalChangeActionsFailed();

protected:
    /// The monochromator being controlled.
    BioXASMainMonochromator *mono_;
    /// The current state of the control.
    State state_;

private:
    /// Disconnects from the given action and marks it for deletion.
    void deleteAction(QObject *crystalChangeAction);
    /// Handles updating the state to match present action and mono conditions.
    void updateState();

};

#endif // BIOXASMAINMONOCHROMATORCRYSTALCHANGECONTROL_H
