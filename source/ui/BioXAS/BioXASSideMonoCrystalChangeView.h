#ifndef BIOXASSIDEMONOCRYSTALCHANGEVIEW_H
#define BIOXASSIDEMONOCRYSTALCHANGEVIEW_H

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QPushButton>

#include "beamline/BioXAS/BioXASSideMonochromator.h"

class BioXASSideMonoCrystalChangeStep;
class BioXASSideMonoCrystalChangeStepView;

class BioXASSideMonoCrystalChangeView : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASSideMonoCrystalChangeView(BioXASSideMonochromator *toView, QWidget *parent = 0);
    /// Destructor.
    virtual ~BioXASSideMonoCrystalChangeView();

    /// Returns the monochromator being viewed.
    BioXASSideMonochromator* mono() const;

signals:
    /// Notifier that the mono being viewed has changed.
    void monoChanged(BioXASSideMonochromator *newMono);

public slots:
    /// Sets the monochromator to be viewed.
    void setMono(BioXASSideMonochromator *toView);

protected slots:
    /// Handles monochromator connection changes.
    void onMonoConnectedChanged();

protected:
    /// Creates the action steps from the mono's available actions.
    void createSteps();
    /// Creates the UI.
    void createUI();

protected:
    BioXASSideMonochromator *mono_;

    BioXASSideMonoCrystalChangeStep *closeSlits_;
    BioXASSideMonoCrystalChangeStep *removePaddle_;
    BioXASSideMonoCrystalChangeStep *turnKeyCCW_;
    BioXASSideMonoCrystalChangeStep *toCrystalChangePosition_;
    BioXASSideMonoCrystalChangeStep *disableBrake_;
    BioXASSideMonoCrystalChangeStep *fromCrystalChangePosition_;
    BioXASSideMonoCrystalChangeStep *enableBrake_;
    BioXASSideMonoCrystalChangeStep *toNewRegion_;
    BioXASSideMonoCrystalChangeStep *turnKeyCW_;

    QList<BioXASSideMonoCrystalChangeStep*> steps_;

};




class BioXASSideMonoCrystalChangeStepView : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASSideMonoCrystalChangeStepView(BioXASSideMonoCrystalChangeStep *toView, QWidget *parent = 0);
    /// Destructor.
    virtual ~BioXASSideMonoCrystalChangeStepView();

    /// Returns the step being viewed.
    BioXASSideMonoCrystalChangeStep* step() const;
    /// Returns whether the step is extended.
    bool isExtended() const;

signals:
    /// Notifier that the step in this view has changed.
    void stepChanged(BioXASSideMonoCrystalChangeStep *newStep);
    /// Notifier that the view is/isn't now extended.
    void extendedChanged(bool isExtended);

public slots:
    /// Sets the step being viewed.
    void setStep(BioXASSideMonoCrystalChangeStep* newStep);
    /// Sets whether the view is extended.
    void setExtended(bool isExtended);

protected slots:
    /// Handles toggling view extension when moreButton is clicked.
    void onMoreButtonClicked();
    /// Handles changes in the step action.
    void onStepActionChanged();
    /// Handles changes in step number.
    void onStepNumberChanged();
    /// Handles changes in the step basic instructions.
    void onStepBasicInstructionsChanged();
    /// Handles changes in the step detailed instructions.
    void onStepDetailedInstructionsChanged();
    /// Handles view chanes for when the step has started.
    void onStepStarted();
    /// Handles view changes for when the step has succeeded.
    void onStepSucceeded();
    /// Handles view changes for when the step has failed.
    void onStepFailed();

protected:
    BioXASSideMonoCrystalChangeStep* step_;
    QLabel *stepNumber_;
    QLabel *stepBasicInstructions_;
    QPushButton *moreButton_;
    QLabel *stepDetailedInstructions_;

    bool isExtended_;
};




class BioXASSideMonoCrystalChangeStep : public QObject
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASSideMonoCrystalChangeStep(AMAction3 *action, QObject *parent = 0);
    /// Destructor.
    virtual ~BioXASSideMonoCrystalChangeStep();

    /// Returns the AMAction3 of the step in the crystal change process.
    AMAction3* action() const;
    /// Returns the step number.
    int number() const;
    /// Returns the basic instructions for this step.
    QString basicInstructions() const;
    /// Returns the detailed instructions for this step.
    QString detailedInstructions() const;


signals:
    /// Notifier that the action for this step has changed.
    void actionChanged(AMAction3* newAction);
    /// Notifier that the step number has changed.
    void numberChanged(int newNumber);
    /// Notifier that the basic instructions for this step have changed.
    void basicInstructionsChanged(const QString &newInstructions);
    /// Notifier that the detailed instructions for this step have changed.
    void detailedInstructionsChanged(const QString &newInstructions);
    /// Notifier that the action for this step has started.
    void stepStarted();
    /// Notifier that the action for this step has succeeded.
    void stepSucceeded();
    /// Notifier that the action for this step has failed.
    void stepFailed();

public slots:
    /// Sets the action for this step.
    void setAction(AMAction3* newAction);
    /// Sets the step number for this step.
    void setNumber(int newNumber);
    /// Sets the basic instructions for this step.
    void setBasicInstructions(const QString &newInstructions);
    /// Sets the detailed instructions for this step.
    void setDetailedInstructions(const QString &newInstructions);

protected:
    AMAction3* action_;
    int number_;
    QString basicInstructions_;
    QString detailedInstructions_;

};

#endif // BIOXASSIDEMONOCRYSTALCHANGEVIEW_H
