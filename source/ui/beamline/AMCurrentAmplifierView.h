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

signals:
    void currentAmplifierChanged(AMCurrentAmplifier *newAmplifier);

public slots:
    void setCurrentAmplifier(AMCurrentAmplifier *amplifier);

protected slots:
    /// Handles passing changes in the value combo box to the amplifier.
    virtual void onValueComboBoxChanged(int index) = 0;
    /// Handles passing changes in the units combo box to the amplifier.
    virtual void onUnitsComboBoxChanged(int index) = 0;
    /// Handles setting the value combo box when the amplifier's value has been changed independently.
    virtual void onValueChanged(int value) = 0;
    /// Handles setting the units combo box when the amplifier's units have been changed independently.
    virtual void onUnitsChanged(QString units) = 0;

protected:
    /// The pointer to the current amplifier this view manages.
    AMCurrentAmplifier *amplifier_;

    /// Combo box holding the amplifier value.
    QComboBox *value_;
    /// Combo box holding the amplifier units.
    QComboBox *units_;

//    /// The tool button for the minus button.
//    QToolButton *minus_;
//    /// The tool button for the plus button.
//    QToolButton *plus_;
};

#endif // AMCURRENTAMPLIFIERVIEW_H
