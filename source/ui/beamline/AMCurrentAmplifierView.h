#ifndef AMCURRENTAMPLIFIERVIEW_H
#define AMCURRENTAMPLIFIERVIEW_H

#include <QWidget>
#include <QComboBox>
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
    explicit AMCurrentAmplifierView(AMCurrentAmplifier *amplifier, QWidget *parent = 0);
    virtual ~AMCurrentAmplifierView();

    /// Returns the pointer to the AMCurrentAmplifier this view is visualizing.
    AMCurrentAmplifier *currentAmplifier() const;

    /// Returns the view mode currently being displayed.
    AMCurrentAmplifier::AmplifierMode viewMode() const;

signals:
    void currentAmplifierChanged(AMCurrentAmplifier *newAmplifier);

public slots:
    void setCurrentAmplifier(AMCurrentAmplifier *amplifier);
    void setViewMode(AMCurrentAmplifier::AmplifierMode newMode);
    void refreshDisplayValues();
    void refreshButtons();

protected slots:
    /// Handles passing changes in the value combo box to the amplifier.
    void onValueComboBoxChanged(int index);
    /// Handles passing changes in the units combo box to the amplifier.
    void onUnitsComboBoxChanged(int index);

    void onMinusClicked();
    void onPlusClicked();

    virtual void onCustomContextMenuRequested(QPoint position);

protected:
    /// The pointer to the current amplifier this view manages.
    AMCurrentAmplifier *amplifier_;
    /// The current view mode.
    AMCurrentAmplifier::AmplifierMode viewMode_;
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
