#ifndef BIOXASSIDEMONOCRYSTALCHANGEVIEW_H
#define BIOXASSIDEMONOCRYSTALCHANGEVIEW_H

#include <QWidget>
#include <QGroupBox>
#include <QScrollArea>
#include <QDialog>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>

#include "beamline/BioXAS/BioXASSideMonochromator.h"
#include "acquaman/BioXASSide/BioXASSideMonoCrystalChangeSteps.h"

class BioXASSideMonoCrystalChangeStepsView;
class BioXASSideMonoCrystalChangeFinalView;
class BioXASSideMonoCrystalChangeView : public QDialog
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASSideMonoCrystalChangeView(BioXASSideMonochromator *mono);
    /// Destructor.
    virtual ~BioXASSideMonoCrystalChangeView();

    /// Returns the mono being viewed.
    BioXASSideMonochromator* mono() const;

signals:
    /// Notifier that the mono being viewed has changed.
    void monoChanged(BioXASSideMonochromator *newMono);

public slots:
    /// Sets the mono being viewed.
    void setMono(BioXASSideMonochromator *newMono);
    /// Shows the steps view, hides the final view.
    void showStepsView();
    /// Shows the final view with the given message, hides the steps view.
    void showFinalView(const QString &message);

protected slots:
    /// Shows final view with appropriate message when the steps have finished.
    void onStepsFinished();
    /// Shows final view with appropriate message when the steps have failed.
    void onStepsFailed();

protected:
    BioXASSideMonochromator *mono_;
    BioXASSideMonoCrystalChangeSteps *steps_;

    BioXASSideMonoCrystalChangeStepsView *stepsView_;
    BioXASSideMonoCrystalChangeFinalView *finalView_;
};




class BioXASSideMonoCrystalChangeStepView;
class BioXASSideMonoCrystalChangeStepsView : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASSideMonoCrystalChangeStepsView(BioXASSideMonoCrystalChangeSteps *toView, QWidget *parent = 0);
    /// Destructor.
    virtual ~BioXASSideMonoCrystalChangeStepsView();

signals:
    /// Notifier that the steps being viewed have changed.
    void stepsChanged(BioXASSideMonoCrystalChangeSteps *newSteps);

public slots:
    /// Sets the steps being viewed.
    void setSteps(BioXASSideMonoCrystalChangeSteps *steps);

protected slots:
    /// Updates the step view to display information for the current step.
    void onCurrentStepChanged(BioXASSideMonoCrystalChangeStep *newStep);
    /// Updates the progress bar based on steps progress.
    void onProgressUpdate(double numerator, double denominator);

protected:
    /// Steps being viewed.
    BioXASSideMonoCrystalChangeSteps *steps_;
    /// Progress bar.
    QProgressBar *progressBar_;
    /// View of current step.
    BioXASSideMonoCrystalChangeStepView *stepView_;
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
    /// Returns whether the step's long description is shown.
    bool extensionShown() const;

signals:
    /// Notifier that the step being viewed has changed.
    void stepChanged(BioXASSideMonoCrystalChangeStep *newStep);

public slots:
    /// Sets the step being viewed.
    void setStep(BioXASSideMonoCrystalChangeStep* newStep);
    /// Sets the displayed description.
    void setDescription(const QString &description);
    /// Sets special user instructions.
    void setUserInstructions(const QString &instructions);
    /// Sets whether user instructions are shown.
    void setExtensionShown(bool isShown);

protected slots:
    /// Updates displayed basic instructions.
    void onStepDescriptionChanged(const QString &newDescription);
    /// Updates displayed detailed instructions.
    void onStepUserInstructionsChanged(const QString &newInstructions);

protected:
    BioXASSideMonoCrystalChangeStep* step_;

    QLabel *description_;
    QLabel *userInstructions_;

    bool extensionShown_;
    QGroupBox *extension_;
};




class BioXASSideMonoCrystalChangeFinalView : public QWidget
{
    Q_OBJECT

public:
    explicit BioXASSideMonoCrystalChangeFinalView(const QString &message, QWidget *parent = 0);
    virtual ~BioXASSideMonoCrystalChangeFinalView();

signals:
    /// Notifier that the message displayed has changed.
    void messageChanged(const QString &newMessage);
    /// Notifier that the button was clicked.
    void buttonClicked();

public slots:
    /// Sets the displayed message.
    void setMessage(const QString &message);

protected:
    QLabel *message_;
    QPushButton *button_;
};

#endif // BIOXASSIDEMONOCRYSTALCHANGEVIEW_H
