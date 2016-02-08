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
#include <QToolButton>
#include <QComboBox>
#include <QLabel>
#include <QLayout>
#include <QMenu>

#include "beamline/AMCurrentAmplifier.h"

class AMCurrentAmplifierView : public QWidget
{
    Q_OBJECT

public:
    /// Simple enum holding whether the view should be either basic or advanced looking.
    enum ViewMode { Basic = 0, Advanced = 1 };

    /// Constructor.
    explicit AMCurrentAmplifierView(QWidget *parent = 0);
    /// Destructor.
    virtual ~AMCurrentAmplifierView();

    /// Returns true if the view is initialized, false otherwise.
    bool initialized() const;
    /// Returns true if the view is valid, false otherwise.
    virtual bool isValid() const = 0;
    /// Returns the mode this view is currently in.
    AMCurrentAmplifierView::ViewMode viewMode() const;
    /// Returns the precision.
    int precision() const;
    /// Returns the format.
    char format() const;
    /// Returns the view's name, an empty string if it doesn't have one.
    QString name() const;

signals:
    /// Emitted when the initialization state of the view changes.
    void initialized(bool);
    /// Emitted when the view mode changes.
    void viewModeChanged(AMCurrentAmplifierView::ViewMode newMode);
    /// Emitted when the precision changes.
    void precisionChanged(int newPrecision);
    /// Emitted when the format changes.
    void formatChanged(char newFormat);
    /// Emitted when the view's name changes.
    void nameChanged(const QString &name);

public slots:
    /// Sets the view mode.
    void setViewMode(AMCurrentAmplifierView::ViewMode newMode);
    /// Sets the precision for values displayed.
    void setPrecision(int newPrecision);
    /// Sets the format for values displayed. Note that CLSSR570 amplifiers cannot accept input in scientific notation--the format_ for these amplifier views must be 'f'.
    void setFormat(char newFormat);
    /// Sets the view's name.
    void setName(const QString &newName);
    /// Shows (or hides) the view name, if it has one.
    void showName(bool show);
    /// Sets the number of values to be displayed at once in the value_ combobox.
    void setViewableValuesMax(int newMax);

protected slots:
    /// Sets the initialized_ state of the view.
    void setInitialized(bool isInitialized);
    /// Called when the selected value in value_ changes. Checks that the view is valid, then calls onValueComboBoxChangedImplementation.
    void onValueComboBoxChanged(const QString &newText);
    /// Called when minus_ button is clicked. Checks that the view is valid, then calls onMinusClickedImplementation.
    void onMinusClicked();
    /// Called when plus_ button is clicked. Checks that the view is valid, then calls onPlusClickedImplementation.
    void onPlusClicked();
    /// Children can call this method when its time to refresh the view. Checks that the view is valid, then calls refreshViewImplementation.
    void refreshView();

	/// Provides a custom context menu, used to switch between Basic and Advanced views.
	void onCustomContextMenuRequested(QPoint position);
	/// set the view to basic view
	void onBasicViewActionTriggered();
	/// set the view to advance view
	void onAdvancedViewActionTriggered();

protected:
	/// pure virtual function to handle the value changed signal for the combobox
	virtual void onValueComboBoxChangedImplementation(const QString &newText) = 0;
	/// pure virtual function to handle the minus button click signal
	virtual void onMinusClickedImplementation() = 0;
	/// pure virtual function to handle the plus button click signal
	virtual void onPlusClickedImplementation() = 0;
	/// pure virtual function to handle the refresh view signal
	virtual void refreshViewImplementation() = 0;
	/// the customized implementation for the Amplifier view context menu
	virtual void onCustomContextMenuActionImplementation(QMenu *contextMenu);
	/// Helper function that returns a string of the given amplifier value and units. Provides consistent formatting.
    QString toDisplay(double value, const QString &units) const;

protected:
    /// Bool indicating the view's initialization state. Used to prevent the view from setting values when it shouldn't.
    bool initialized_;
    /// The current view mode.
    ViewMode viewMode_;
    /// The precision to be displayed for each value.
    int precision_;
    /// The format used to display values: 'f' for normal, 'e' for scientific, 'g' for whichever is more concise.
    char format_;
    /// Holds the view's name.
    QLabel *name_;
    /// Minus button.
    QToolButton *minus_;
    /// Plus button.
    QToolButton *plus_;
    /// Displays the value options.
    QComboBox *value_;
    /// The general view layout.
    QHBoxLayout *layout_;

	/// the context menu action for basic view
	QAction *basicViewAction_;
	/// the context menu action for advanced view
	QAction *advancedViewAction_;
};

#endif // AMCURRENTAMPLIFIERVIEW_H
