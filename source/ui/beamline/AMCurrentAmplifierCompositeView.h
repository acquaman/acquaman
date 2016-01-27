#ifndef AMCURRENTAMPLIFIERCOMPOSITEVIEW_H
#define AMCURRENTAMPLIFIERCOMPOSITEVIEW_H

#include <QWidget>

#include "beamline/AMCurrentAmplifier.h"
#include "ui/beamline/AMCurrentAmplifierView.h"

class AMCurrentAmplifierCompositeView : public AMCurrentAmplifierView
{
    Q_OBJECT

public:
    /// Constructor. Takes two AMCurrentAmplifier arguments.
    explicit AMCurrentAmplifierCompositeView(AMCurrentAmplifier *amp1, AMCurrentAmplifier *amp2, QWidget *parent = 0);
    /// Destructor.
    virtual ~AMCurrentAmplifierCompositeView();

    /// Returns the first amplifier.
    AMCurrentAmplifier *amplifier1() const;
    /// Returns the second amplifier.
    AMCurrentAmplifier *amplifier2() const;
    /// Returns true if this view is valid, false otherwise.
    virtual bool isValid() const;

signals:

public slots:

protected slots:
    /// Updates the value_ widget selection to reflect the amplifier's new selection.
    void onAmplifierValueChanged();
    /// Handles passing changes in the value_ combobox to the amplifier.
    virtual void onValueComboBoxChangedImplementation(const QString &newText);
    /// Calls AMCurrentAmplifier::decreaseValue() for both amplifiers.
    virtual void onMinusClickedImplementation();
    /// Calls AMCurrentAmplifier::increaseValue() for both amplifiers.
    virtual void onPlusClickedImplementation();
    /// Clears and repopulations value_ widget.
	virtual void refreshViewImplementation();

protected:
    /// Clears and repopulates value_ widget with information from amplifier1_.
    void refreshValues();
    /// Sets whether buttons should be en/disabled according to the min/max value state of amplifier1_.
    void refreshButtons();

protected:
    /// The first amplifier.
    AMCurrentAmplifier *amplifier1_;
    /// The second amplifier.
    AMCurrentAmplifier *amplifier2_;

};

#endif // AMCURRENTAMPLIFIERCOMPOSITEVIEW_H
