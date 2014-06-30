/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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
    /// Simple enum holding whether the view should be either basic or advanced looking.
    enum ViewMode { Basic = 0, Advanced = 1 };

    /// Constructor. Takes in instance of AMCurrentAmplifier.
    explicit AMCurrentAmplifierView(AMCurrentAmplifier *amplifier, QWidget *parent = 0);
    virtual ~AMCurrentAmplifierView();

    /// Returns the pointer to the AMCurrentAmplifier this view is visualizing.
    AMCurrentAmplifier *currentAmplifier() const;
    /// Returns boolean indicating whether this view has been initialized.
    bool initialized() const;
    /// Returns the current view mode.
    AMCurrentAmplifierView::ViewMode viewMode() const;

signals:
    void viewModeChanged(AMCurrentAmplifierView::ViewMode newMode);

public slots:
    /// Sets the view mode.
    void setViewMode(ViewMode newMode);
    /// Sets whether the amplifier name should be shown.
    void showName(bool showName);
    /// Sets the maximum viewable elements in the combo box. Note that QComboBox::setMaxVisibleItems() is ignored in some combobox styles, such as the Mac style (see Qt documentatation).
    void setViewableValuesMax(int newMax);
    /// Clears and resets view widgets.
    void refreshView();

protected slots:
    /// Handles passing changes in the value combo box to the amplifier.
    void onValueComboBoxChanged(const QString &newText);
    /// Updates the value_ widget selection to reflect amplifier's new selection.
    void onAmplifierValueChanged();
    /// Calls either AMCurrentAmplifier::decreaseGain/decreaseSensitivity depending on the amplifier mode.
    void onMinusClicked();
    /// Calls either AMCurrentAmplifier::increaseGain/increaseSensitivity depending on the amplifier mode.
    void onPlusClicked();
    /// If multiple modes are supported by the amplifier, context menu allows user to select gain/sensitivity preference.
    virtual void onCustomContextMenuRequested(QPoint position);

protected:
    /// Returns a string of the amplifier's value and units. Provides consistent formatting.
    QString valueToString(double value, const QString &units) const;
    /// Clears and repopulates value_ and units_ widget with information from amplifier_.
    void refreshDisplayValues();
    /// Sets whether buttons should be en/disabled according to whether amplifier_ is at a max/min gain/sensitivity state.
    void refreshButtons();

protected:
    /// Boolean indicating whether this view has been initialized.
    bool initialized_;
    /// The pointer to the current amplifier this view manages.
    AMCurrentAmplifier *amplifier_;
    /// Flag holding whether or not the view is basic or advanced.
    ViewMode mode_;
    /// Label to display amplifier name.
    QLabel *name_;
    /// The tool button for the minus button.
    QToolButton *minus_;
    /// The tool button for the plus button.
    QToolButton *plus_;
    /// Combo box holding the available options for amplifier value.
    QComboBox *value_;
};

#endif // AMCURRENTAMPLIFIERVIEW_H
