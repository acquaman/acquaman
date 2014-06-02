#ifndef CLSKEITHLEY428VIEW_H
#define CLSKEITHLEY428VIEW_H

#include <QWidget>
#include <QComboBox>
#include <QToolButton>
#include <QLayout>
#include <QLabel>
#include <QRadioButton>

#include "beamline/CLS/CLSKeithley428.h"
#include "ui/beamline/AMCurrentAmplifierView.h"


class CLSKeithley428View : public QWidget
{
    Q_OBJECT

public:

    /// Constructor. Takes a pointer to the Keithley428 amplifier to view.
    explicit CLSKeithley428View(CLSKeithley428 *amplifier, QWidget *parent = 0);
    /// Destructor
    virtual ~CLSKeithley428View();

    /// Returns the currently displayed value mode.
    AMCurrentAmplifier::AmplifierMode valueMode() const;

signals:
    void amplifierChanged(AMCurrentAmplifier *newAmplifier);
    void amplifierModeChanged();

public slots:
    void setAmplifierMode(AMCurrentAmplifier::AmplifierMode mode);
    void setAmplifier(CLSKeithley428 *amplifier);
    void refreshDisplayValues();

protected slots:
    void onButtonClicked();
    void onAmplifierConnected(bool isConnected);

    /// Handles passing changes in the value combo box to the Keithley428.
    virtual void onValueComboBoxChanged(int newIndex);
    virtual void onUnitsComboBoxChanged(int index);
    /// Handles setting the value combo box when the Keithley428's value has been changed independently.
    virtual void onValueChanged(int valueIndex);
    virtual void onUnitsChanged(QString units);

protected:
    /// Pointer to the Keithley428 amplifier this view manages.
    CLSKeithley428 *amplifier_;
    /// Combo box holding the available values.
    QComboBox *value_;
    /// Label holding the available units.
    QComboBox *units_;

    QRadioButton *gainButton_;
    QRadioButton *sensitivityButton_;


};

#endif // CLSKEITHLEY428VIEW_H
