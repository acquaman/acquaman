#ifndef BIOXASMAINMONOCHROMATORCRYSTALCHANGECONTROL_H
#define BIOXASMAINMONOCHROMATORCRYSTALCHANGECONTROL_H

#include <QObject>

#include "BioXASMainMonochromator.h"

class BioXASMainMonochromatorCrystalChangeControl : public QObject
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASMainMonochromatorCrystalChangeControl(BioXASMainMonochromator *mono, QObject *parent = 0);
    /// Destructor.
    virtual ~BioXASMainMonochromatorCrystalChangeControl();
    /// Returns the monochromator being controlled.
    BioXASMainMonochromator* mono() const { return mono_; }
    /// Returns true if there is a crystal change running, false otherwise.
    bool crystalChangeRunning() const { return crystalChangeRunning_; }

signals:
    /// Notifier that the mono being controlled has changed.
    void monoChanged(BioXASMainMonochromator *newMono);
    /// Notifier that the crystal change actions have started.
    void crystalChangeStarted();
    /// Notifier that the currently running action has changed, and so has the corresponding description and user information.
    void currentActionChanged(const QString &newDescription, const QString &newInformation);
    /// Notifier that the crystal change actions have ended. Argument is true for success, false for fail.
    void crystalChangeEnded(bool success);
    /// Notifier that the crystal change running status has changed.
    void crystalChangeRunningChanged(bool isRunning);

public slots:
    /// Sets the monochromator.
    void setMono(BioXASMainMonochromator *newMono);
    /// Starts the crystal change, if current mono is valid.
    void startCrystalChange();

protected slots:
    /// Sets the crystal change running status.
    void setCrystalChangeRunning(bool isRunning);
    /// Handles when the currently executing subaction has changed.
    void onCrystalChangeSubActionChanged(int actionIndex);
    /// Handles when the crystal change actions have succeeded.
    void onCrystalChangeActionsSucceeded();
    /// Handles situation where crystal change actions report themselves cancelled/failed/destroyed.
    void onCrystalChangeActionsFailed();

protected:
    /// The monochromator being controlled.
    BioXASMainMonochromator *mono_;
    /// Boolean indicating whether a crystal change is running.
    bool crystalChangeRunning_;

private:
    /// Disconnects from the given action and marks it for deletion.
    void deleteAction(QObject *listAction);

};

#endif // BIOXASMAINMONOCHROMATORCRYSTALCHANGECONTROL_H
