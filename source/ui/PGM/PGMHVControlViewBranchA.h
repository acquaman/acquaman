#ifndef PGMHVCONTROLVIEWBRANCHA_H
#define PGMHVCONTROLVIEWBRANCHA_H

#include <QWidget>

class QLabel;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QPushButton;
class QGroupBox;
class QLineEdit;

class PGMHVControlViewBranchA : public QWidget
{
    Q_OBJECT
public:
    /// Constructor.  Note, currently empty. Once PGMHVControl is complete it'll be
    /// passed into this.
    explicit PGMHVControlViewBranchA(QWidget *parent = 0);

signals:

public slots:

protected slots:

protected:

    /// Groupbox containing layout and displaying a title header.
    QGroupBox *branchAHVbox_;
    /// Main Grid layout.
    QGridLayout *mainGridLayout_;
    /// Main vertical layout for spacing reasons.
    QVBoxLayout *mainVLayout_;
    /// Main horizontal layout for space reasons.
    QHBoxLayout *mainHLayout_;
    /// I0 Beamline detector bias layout
    QHBoxLayout *i0BeamlineBiasLayout_;
    /// I0 Endtation detector bias layout
    QHBoxLayout *i0EndstationBiasLayout_;
    /// TEY bias layout
    QHBoxLayout *teyBiasLayout_;
    /// Fluorescence detector 1 layout
    QHBoxLayout *fluoDetectorLayout1_;
    /// Fluorescence detector 2 layout
    QHBoxLayout *fluoDetectorLayout2_;
    /// Fluorescence detector 3 layout
    QHBoxLayout *fluoDetectorLayout3_;

    /// Labels for devices
    QLabel *positiveHVLabel_;
    QLabel *negativeHVLabel_;
    QLabel *i0BeamlineBiasLabel_;
    QLabel *i0EndstationBiasLabel_;
    QLabel *teyBiasLabel_;
    QLabel *flDetector1Label_;
    QLabel *flDetector2Label_;
    QLabel *flDetector3Label_;

    /// Header labels for layout
    QLabel *deviceLabel_;
    QLabel *setpointLabel_;
    QLabel *onOffLabel_;
    QLabel *actualVoltageLabel_;

    /// Labels for actual voltage values
    QLabel *i0BeamlineBiasValue_;
    QLabel *i0EndstationBiasValue_;
    QLabel *teyBiasValue_;
    QLabel *flDetector1Value_;
    QLabel *flDetector2Value_;
    QLabel *flDetector3Value_;

    /// Input widgets for setpoints
    QLineEdit *i0BeamlineSetpoint_;
    QLineEdit *i0EndstationSetpoint_;
    QLineEdit *teySetpoint_;
    QLineEdit *flDetector1Setpoint_;
    QLineEdit *flDetector2Setpoint_;
    QLineEdit *flDetector3Setpoint_;

    /// On/off and status pushbutton for devices
    QPushButton *i0BeamlineVoltageOnOffButton_;
    QPushButton *i0EndstationVoltageOnOffButton_;
    QPushButton *teyOnOffButton_;
    QPushButton *flDetector1OnOffButton_;
    QPushButton *flDetector2OnOffButton_;
    QPushButton *flDetector3OnOffButton_;

};

#endif // PGMHVCONTROLVIEWBRANCHA_H
