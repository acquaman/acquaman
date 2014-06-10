#ifndef AMCURRENTAMPLIFIERVIEW_H
#define AMCURRENTAMPLIFIERVIEW_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QToolButton>
#include <QLayout>
#include <QDebug>
#include <QMenu>

#include "beamline/AMCurrentAmplifier.h"

class AMCurrentAmplifierView : public QWidget
{
    Q_OBJECT

public:
    /// Constructor. Takes in instance of AMCurrentAmplifier.
    explicit AMCurrentAmplifierView(AMCurrentAmplifier *amplifier, bool showName = false, QWidget *parent = 0);
    virtual ~AMCurrentAmplifierView();

    /// Returns the pointer to the AMCurrentAmplifier this view is visualizing.
    AMCurrentAmplifier *currentAmplifier() const;

    /// Returns boolean indicating whether this view has been initialized.
    bool initialized() const;

public slots:
    /// Clears and resets view widgets.
    void refreshView();

protected slots:
    /// Handles passing changes in the value combo box to the amplifier.
    void onValueComboBoxChanged(int index);
    /// Handles passing changes in the units combo box to the amplifier.
    void onUnitsComboBoxChanged(int index);
    /// Updates the value_ widget selection to reflect amplifier's new selection.
    void onAmplifierValueChanged(int valueIndex);
    /// Updates the units_ widget selection to reflect amplifier's new selection.
    void onAmplifierUnitsChanged(int unitsIndex);
    /// Calls either AMCurrentAmplifier::decreaseGain/decreaseSensitivity depending on the amplifier mode.
    void onMinusClicked();
    /// Calls either AMCurrentAmplifier::increaseGain/increaseSensitivity depending on the amplifier mode.
    void onPlusClicked();
    /// If multiple modes are supported by the amplifier, context menu allows user to select gain/sensitivity preference.
    virtual void onCustomContextMenuRequested(QPoint position);

protected:
    /// Clears and repopulates value_ and units_ widget with information from amplifier_.
    void refreshDisplayValues();
    /// Sets whether buttons should be en/disabled according to whether amplifier_ is at a max/min gain/sensitivity state.
    void refreshButtons();

protected:
    /// Boolean indicating whether this view has been initialized.
    bool initialized_;
    /// The pointer to the current amplifier this view manages.
    AMCurrentAmplifier *amplifier_;
    /// Combo box holding the amplifier value.
    QComboBox *value_;
    /// Combo box holding the amplifier units.
    QComboBox *units_;
    /// The tool button for the minus button.
    QToolButton *minus_;
    /// The tool button for the plus button.
    QToolButton *plus_;
};

#endif // AMCURRENTAMPLIFIERVIEW_H
