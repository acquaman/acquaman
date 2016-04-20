#ifndef PGMBLADECURRENTVIEW_H
#define PGMBLADECURRENTVIEW_H

#include <QWidget>

class QLabel;
class QGroupBox;

/// This view class display common blade current information for both
/// branches. Contains blade currents for both exit slits, entrance slit, TEY,
/// FLY, I0 Beamline and I0 Endstation.
class PGMBladeCurrentView : public QWidget
{
    Q_OBJECT
public:
    /// Constructor.
    explicit PGMBladeCurrentView(QWidget *parent = 0);

signals:

public slots:

protected slots:
    /// Handles exit slit lower blade current change on branch A.
    void onExitSlitLowerBladeCurrentAChange(double value);
    /// Handles exit slit upper blade current change on branch A.
    void onExitSlitUpperBladeCurrentAChange(double value);
    /// Handles exit slit lower blade current change on branch B.
    void onExitSlitLowerBladeCurrentBChange(double value);
    /// Handles exit slit upper blade current change on branch B.
    void onExitSlitUpperBladeCurrentBChange(double value);
    /// Handles entrance slit lower blade current change.
    void onEntranceSlitLowerBladeCurrentChange(double value);
    /// Handles entrance slit upper blade current change.
    void onEntranceSlitUpperBladeCurrentChange(double value);
    /// Handles TEY blade current change.
    void onTEYBladeCurrentChange(double value);
    /// Handles FLY blade current change.
    void onFLYBladeCurrentChange(double value);
    /// Handles I0 endstation blade current change.
    void onI0EndstationBladeCurrentChange(double value);
    /// Handles I0 Beamline blade current change.
    void onI0BeamlineBladeCurrentChange(double value);
    /// Handles Photodiode blade current change.
    void onPhotodiodeBladeCurrentChange(double value);


protected:
    /// Labels for the exit slit upper/lower blades for branch A and B.
    QLabel *exitSlitLowerBladeCurrentALabel_;
    QLabel *exitSlitUpperBladeCurrentALabel_;
    QLabel *exitSlitLowerBladeCurrentBLabel_;
    QLabel *exitSlitUpperBladeCurrentBLabel_;
    /// The values of the current in the exit slit blades for branch A and B.
    QLabel *exitSlitLowerBladeCurrentAValue_;
    QLabel *exitSlitUpperBladeCurrentAValue_;
    QLabel *exitSlitLowerBladeCurrentBValue_;
    QLabel *exitSlitUpperBladeCurrentBValue_;
    /// Labels for the entrance slit blade currents.
    QLabel *entranceSlitLowerBladeCurrentLabel_;
    QLabel *entranceSlitUpperBladeCurrentLabel_;
    /// Values for entrance slit blade currents.
    QLabel *entranceSlitLowerBladeCurrentValue_;
    QLabel *entranceSlitUpperBladeCurrentValue_;
    /// TEY blade current label.
    QLabel *teyBladeCurrentLabel_;
    /// FLY blade current label.
    QLabel *flyBladeCurrentLabel_;
    /// I0 Endstation blade current label.
    QLabel *i0EndstationBladeCurrentLabel_;
    /// I0 Beamline Control blade current label.
    QLabel *i0BeamlineBladeCurrentLabel_;
    /// Photodiode Blade current label.
    QLabel *photodiodeBladeCurrentLabel_;
    /// TEY blade current value.
    QLabel *teyBladeCurrentValue_;
    /// FLY blade current value.
    QLabel *flyBladeCurrentValue_;
    /// I0 Endstation blade current value.
    QLabel *i0EndstationBladeCurrentValue_;
    /// I0 Beamline blade current value.
    QLabel *i0BeamlineBladeCurrentValue_;
    /// Photodiode blade current value.
    QLabel *photodiodeBladeCurrentValue_;
};

#endif // PGMBLADECURRENTVIEW_H
