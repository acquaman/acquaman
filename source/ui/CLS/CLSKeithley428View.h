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
    CLSKeithley428::AmplifierMode valueMode() const;

    /// Returns the current amplifier being viewed.
    CLSKeithley428* currentAmplifier() const;

signals:
    void amplifierChanged();
    void viewModeChanged();

public slots:
    void setValueMode(CLSKeithley428::AmplifierMode mode);
    void setAmplifier(CLSKeithley428 *amplifier);
    void refreshDisplayValues();

protected slots:
    void onButtonClicked();
    /// Handles passing changes in the value combo box to the Keithley428.
    virtual void onValueComboBoxChanged(int newIndex);
    /// Handles setting the value combo box when the Keithley428's value has been changed independently.
    virtual void onValueChanged(int valueIndex);

protected:
    /// Pointer to the Keithley428 amplifier this view manages.
    CLSKeithley428 *amplifier_;
    /// Combo box holding the available values.
    QComboBox *value_;
    /// Label holding the available units.
    QLabel *units_;

    QRadioButton *gainButton_;
    QRadioButton *sensitivityButton_;


};

#endif // CLSKEITHLEY428VIEW_H
