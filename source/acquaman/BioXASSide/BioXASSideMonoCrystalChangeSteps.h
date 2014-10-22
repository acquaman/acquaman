#ifndef BIOXASSIDEMONOCRYSTALCHANGESTEPS_H
#define BIOXASSIDEMONOCRYSTALCHANGESTEPS_H

#include <QObject>
#include <QDebug>

#include "source/actions3/AMAction3.h"
#include "source/actions3/AMListAction3.h"

class BioXASSideMonoCrystalChangeStep;

class BioXASSideMonoCrystalChangeSteps : public QObject
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASSideMonoCrystalChangeSteps(AMAction3 *crystalChangeActions, QObject *parent);
    /// Destructor.
    virtual ~BioXASSideMonoCrystalChangeSteps();

    /// Returns the current step index.
    int currentStepIndex() const;
    /// Returns the current step.
    BioXASSideMonoCrystalChangeStep* currentStep() const;

signals:
    /// Notifier that the crystal change actions list has been changed.
    void actionsChanged(AMAction3 *newActions);
    /// Notifier that the steps have begun execution.
    void stepsStarted();
    /// Notifier that the current step has changed.
    void currentStepChanged(BioXASSideMonoCrystalChangeStep *newStep);
    /// Notifier that the progress has changed.
    void progressChanged(double numerator, double denominator);
    /// Notifier that the steps have succeeded.
    void stepsSucceeded();
    /// Notifier that the steps have failed.
    void stepsFailed();

public slots:
    /// Sets the crystal change actions list.
    void setActions(AMAction3 *newActions);
    /// Sets the current step index.
    void setCurrentStepIndex(int newIndex);
    /// Initiates execution of the actions list.
    void start();

protected slots:
    /// Handles changing the current step when current action has changed.
    void onCurrentActionChanged(int actionIndex);

protected:
    AMListAction3 *actions_;

    int currentStepIndex_;
    QList<BioXASSideMonoCrystalChangeStep*> steps_;

    BioXASSideMonoCrystalChangeStep *closeSlits_;
    BioXASSideMonoCrystalChangeStep *removePaddle_;
    BioXASSideMonoCrystalChangeStep *enableKey_;
    BioXASSideMonoCrystalChangeStep *moveToCCPosition_;
    BioXASSideMonoCrystalChangeStep *disableBrake_;
    BioXASSideMonoCrystalChangeStep *switchCrystals_;
    BioXASSideMonoCrystalChangeStep *enableBrake_;
    BioXASSideMonoCrystalChangeStep *moveIntoNewRegion_;
    BioXASSideMonoCrystalChangeStep *disableKey_;
};




class BioXASSideMonoCrystalChangeStep : public QObject
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASSideMonoCrystalChangeStep(AMAction3 *action, const QString &description, const QString &userInstructions, QObject *parent = 0);
    /// Destructor.
    virtual ~BioXASSideMonoCrystalChangeStep();

    /// Returns the action for this step.
    AMAction3* action() const { return action_; }
    /// Returns a brief description of this step.
    QString description() const { return description_; }
    /// Returns instructions for a user to perform.
    QString userInstructions() const { return userInstructions_; }

signals:
    /// Notifier that the action has changed.
    void actionChanged(AMAction3 *newAction);
    /// Notifier that the description has changed.
    void descriptionChanged(const QString &newMessage);
    /// Notifier that the user instructions have changed.
    void userInstructionsChanged(const QString &newInstructions);

public slots:
    /// Sets the action.
    void setAction(AMAction3 *newAction);
    /// Sets the step description.
    void setDescription(const QString &newDescription);
    /// Sets instructions for the user related to this step (if any).
    void setUserInstructions(const QString &newInstructions);

protected slots:
    /// Updates the status when the action has succeeded.
    void onActionSucceeded();

protected:
    AMAction3 *action_;
    QString description_;
    QString userInstructions_;
    QString status_;
};

#endif // BIOXASSIDEMONOCRYSTALCHANGESTEPS_H
