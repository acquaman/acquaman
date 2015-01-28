#ifndef BIOXASMAINMONOCHROMATORCRYSTALCHANGECONTROLVIEW_H
#define BIOXASMAINMONOCHROMATORCRYSTALCHANGECONTROLVIEW_H

#include <QWidget>
#include <QMessageBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
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

protected slots:
    /// Pokes the control to start crystal change procedure.
    void toStartCrystalChange();
    /// Updates the view in response to a change in the control's state.
    void onControlStateChanged(BioXASMainMonochromatorCrystalChangeControl::State newState);
    /// Updates the description and user information when the control reports a change in the current action.
    void onControlCurrentActionChanged(const QString &newDescription, const QString &newInformation);
    /// Updates the progress bar when the control reports progress change.
    void onControlProgressChanged(double numerator, double denominator);

protected:
    /// The crystal change control being viewed.
    BioXASMainMonochromatorCrystalChangeControl *control_;

    /// The intro display.
    QDialog *introDisplay_;
    QString introMessage_;

    /// The success display.
    QString successMessage_;

    /// The fail display.
    QDialog *failDisplay_;
    QString failMessage_;

private:
    /// Shows the intro display and hides the others.
    void showIntroDisplay();
    /// Shows the fail display and hides the others.
    void showFailDisplay();

};

#endif // BIOXASMAINMONOCHROMATORCRYSTALCHANGECONTROLVIEW_H
