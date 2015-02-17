#ifndef BIOXASMAINMONOCHROMATORCRYSTALCHANGECONTROLVIEW_H
#define BIOXASMAINMONOCHROMATORCRYSTALCHANGECONTROLVIEW_H

#include <QWidget>
#include <QMessageBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QTextEdit>
#include <QProgressBar>
#include <QLayout>

#include "beamline/BioXAS/BioXASMainMonochromatorCrystalChangeControl.h"

class BioXASMainMonochromatorCrystalChangeControlView : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASMainMonochromatorCrystalChangeControlView(BioXASMainMonochromatorCrystalChangeControl *control, QWidget *parent);
    /// Destructor.
    virtual ~BioXASMainMonochromatorCrystalChangeControlView();

signals:
    /// Notifier that the crystal change control being viewed has changed.
    void controlChanged(BioXASMainMonochromatorCrystalChangeControl *newControl);

public slots:
    /// Sets the crystal change control being viewed.
    void setControl(BioXASMainMonochromatorCrystalChangeControl *newControl);
    /// Resets the crystal change control state and closes the view.
    void close();

protected slots:
    /// Pokes the control to start crystal change procedure.
    void toStartCrystalChange();
    /// Updates the view in response to a change in the control's state.
    void onControlStateChanged();
    /// Updates the description and user information when the control reports a change in the current action.
    void onControlCurrentActionChanged(const QString &newDescription, const QString &newInstructions);
    /// Updates the progress bar when the control reports progress change.
    void onControlProgressChanged(double numerator, double denominator);

protected:
    /// The crystal change control being viewed.
    BioXASMainMonochromatorCrystalChangeControl *control_;

    /// The initialized display.
    QWidget *initializedDisplay_;
    QString introMessage_;

    /// The running display.
    QWidget *runningDisplay_;
    QProgressBar *runningDisplayProgress_;
    QLabel *runningDisplayDescription_;
    QTextEdit *runningDisplayInstruction_;

    /// The complete success display.
    QWidget *completeSuccessDisplay_;
    QString successMessage_;

    /// The complete fail display.
    QWidget *completeFailDisplay_;
    QString failMessage_;

private:
    /// Shows the intro display and hides the others.
    void showInitializedDisplay();
    /// Shows the actions running display and hides the others.
    void showRunningDisplay();
    /// Shows the success display and hides the others.
    void showCompleteSuccessDisplay();
    /// Shows the fail display and hides the others.
    void showCompleteFailDisplay();

};

#endif // BIOXASMAINMONOCHROMATORCRYSTALCHANGECONTROLVIEW_H
