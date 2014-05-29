#ifndef CLSKEITHLEY428VIEW_H
#define CLSKEITHLEY428VIEW_H

#include <QWidget>
#include <QComboBox>
#include <QToolButton>
#include <QLayout>
#include <QLabel>

#include "beamline/CLS/CLSKeithley428.h"
#include "ui/beamline/AMCurrentAmplifierView.h"

class CLSKeithley428View;
class CLSKeithley428ValueMap;

class CLSKeithley428ContainerView : public QTabWidget
{
    Q_OBJECT

public:
    explicit CLSKeithley428ContainerView(CLSKeithley428 *amplifier, QWidget *parent = 0);
    ~CLSKeithley428ContainerView();

protected:

protected:
    CLSKeithley428View *gainTab_;
    CLSKeithley428View *sensitivityTab_;
};


class CLSKeithley428View : public QWidget
{
    Q_OBJECT

public:

    /// Constructor. Takes a pointer to the Keithley428 amplifier to view.
    explicit CLSKeithley428View(CLSKeithley428 *amplifier, CLSKeithley428ValueMap::ValueMode mode, QWidget *parent = 0);
    /// Destructor
    virtual ~CLSKeithley428View();

    /// Returns the currently displayed value mode.
    CLSKeithley428ValueMap::ValueMode valueMode() const;

    /// Returns the current amplifier being viewed.
    CLSKeithley428* currentAmplifier() const;

signals:
    void valueModeChanged(CLSKeithley428ValueMap::ValueMode newMode);

public slots:
    void setValueMode(CLSKeithley428ValueMap::ValueMode mode);

protected slots:
    /// Handles passing changes in the value combo box to the Keithley428.
    virtual void onValueComboBoxChanged(int newIndex);
    /// Handles setting the value combo box when the Keithley428's value has been changed independently.
    virtual void onValueChanged(int valueIndex);
    /// Handles setting the units label when the Keithley428's units have been changed independently.
    virtual void onUnitsChanged(CLSKeithley428ValueMap::ValueMode mode, const QString &units);

protected:
    /// Pointer to the Keithley428 amplifier this view manages.
    CLSKeithley428 *amplifier_;
    /// Holder for the desired amplifier view: gain or sensitivity.
    CLSKeithley428ValueMap::ValueMode valueMode_;
    /// Combo box holding the available values.
    QComboBox *value_;
    /// Label holding the available units.
    QLabel *units_;


};

#endif // CLSKEITHLEY428VIEW_H
