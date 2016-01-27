#ifndef AMCURRENTAMPLIFIERSINGLEVIEW_H
#define AMCURRENTAMPLIFIERSINGLEVIEW_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QToolButton>
#include <QLayout>
#include <QDebug>
#include <QMenu>

#include "beamline/AMCurrentAmplifier.h"
#include "ui/beamline/AMCurrentAmplifierView.h"

class AMCurrentAmplifierSingleView : public AMCurrentAmplifierView
{
    Q_OBJECT

public:
    /// Constructor. Takes in instance of AMCurrentAmplifier.
    explicit AMCurrentAmplifierSingleView(AMCurrentAmplifier *amplifier, QWidget *parent = 0);
    /// Destructor.
    virtual ~AMCurrentAmplifierSingleView();

    /// Returns the pointer to the AMCurrentAmplifier this view is visualizing.
    AMCurrentAmplifier *amplifier() const;
    /// Returns true if this view is valid, false otherwise.
    virtual bool isValid() const;

signals:

public slots:

protected slots:
    /// Updates the value_ widget selection to reflect amplifier's new selection.
    void onAmplifierValueChanged();
    /// Handles passing changes in the value combo box to the amplifier.
    virtual void onValueComboBoxChangedImplementation(const QString &newText);
    /// Calls either AMCurrentAmplifier::decreaseGain/decreaseSensitivity depending on the amplifier mode.
    virtual void onMinusClickedImplementation();
    /// Calls either AMCurrentAmplifier::increaseGain/increaseSensitivity depending on the amplifier mode.
    virtual void onPlusClickedImplementation();
    /// Clears and resets view widgets.
    virtual void refreshViewImplementation();
    /// If multiple modes are supported by the amplifier, context menu allows user to select gain/sensitivity preference.
	virtual void onCustomContextMenuActionImplementation(QMenu *contextMenu);
	/// slot to set gain preference
	void onGainViewActionTriggered();
	/// slot to set sensitivity preference
	void onSensitivityViewActionTriggered();

protected:
    /// Clears and repopulates value_ widget with information from amplifier_.
    void refreshValues();
    /// Sets whether buttons should be en/disabled according to whether amplifier_ is at a max/min gain/sensitivity state.
    void refreshButtons();

protected:
    /// The pointer to the current amplifier this view manages.
    AMCurrentAmplifier *amplifier_;

	/// The context menu action for gain view
	QAction *gainViewAction_;
	/// The context menu action for sensitivity view
	QAction *sensitivityViewAction_;
};

#endif // AMCURRENTAMPLIFIERSINGLEVIEW_H
