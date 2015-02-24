#ifndef BIOXASMAINMONOCHROMATORCRYSTALCHANGECONTROLVIEW_H
#define BIOXASMAINMONOCHROMATORCRYSTALCHANGECONTROLVIEW_H

#include <QWidget>
#include <QMessageBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QTextEdit>
#include <QProgressBar>
#include <QComboBox>
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

protected slots:
	/// Sets the description label text in the running display.
	void setRunningDisplayDescription(const QString &newDescription);
	/// Sets the instruction label text in the running display. If the new instruction is empty, the instruction widget is hidden from view.
	void setRunningDisplayInstruction(const QString &newInstruction);
	/// Sets the control's next region to be the opposite of it's current region (A -> B, B -> A). Shows selection display if the current region is None.
	void onInitializedDisplayAccepted();
	/// Sets the control's next region using the selection display input.
	void onSelectionDisplayAccepted();
	/// Updates the view in response to a change in the control's current step.
	void onControlStepChanged();
    /// Updates the progress bar when the control reports progress change.
    void onControlProgressChanged(double numerator, double denominator);
	/// Shows the success or failure display, according to the 'success' argument.
	void onControlCrystalChangeComplete(bool success);

protected:
	/// Signals to the control to start a crystal change.
	void startCrystalChange();

protected:
    /// The crystal change control being viewed.
    BioXASMainMonochromatorCrystalChangeControl *control_;

    /// The initialized display.
    QWidget *initializedDisplay_;

	/// The region selection display.
	QWidget *selectionDisplay_;
	QComboBox *selectionDisplayOptions_;

    /// The running display.
    QWidget *runningDisplay_;
    QProgressBar *runningDisplayProgress_;
    QLabel *runningDisplayDescription_;
    QTextEdit *runningDisplayInstruction_;

    /// The complete success display.
    QWidget *completeSuccessDisplay_;

    /// The complete fail display.
    QWidget *completeFailDisplay_;

private:
    /// Shows the intro display and hides the others.
    void showInitializedDisplay();
	/// Shows the selection display and hides the others.
	void showSelectionDisplay();
    /// Shows the actions running display and hides the others.
    void showRunningDisplay();
    /// Shows the success display and hides the others.
    void showCompleteSuccessDisplay();
    /// Shows the fail display and hides the others.
    void showCompleteFailDisplay();

};

#endif // BIOXASMAINMONOCHROMATORCRYSTALCHANGECONTROLVIEW_H
