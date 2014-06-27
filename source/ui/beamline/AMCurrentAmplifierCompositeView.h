#ifndef AMCURRENTAMPLIFIERCOMPOSITEVIEW_H
#define AMCURRENTAMPLIFIERCOMPOSITEVIEW_H

#include <QWidget>

#include "beamline/AMCurrentAmplifier.h"
#include "ui/beamline/AMCurrentAmplifierView.h"

class AMCurrentAmplifierCompositeView : public AMCurrentAmplifierView
{
    Q_OBJECT

public:
    /// Constructor.
    explicit AMCurrentAmplifierCompositeView(AMCurrentAmplifier *amp1, AMCurrentAmplifier *amp2, QWidget *parent = 0);
    /// Destructor.
    virtual ~AMCurrentAmplifierCompositeView();

    /// Returns the first amplifier.
    AMCurrentAmplifier *amplifier1() const;
    /// Returns the second amplifier.
    AMCurrentAmplifier *amplifier2() const;
    /// Returns boolean indicating whether this view is valid.
    bool isValid() const;

signals:

public slots:

protected slots:
    virtual void onValueComboBoxChanged(const QString &newText);
    virtual void onMinusClicked();
    virtual void onPlusClicked();

    void onAmplifierValueChanged();

protected:
    QString valueToString(double value, const QString &units) const;

protected:
    /// The first amplifier.
    AMCurrentAmplifier *amplifier1_;
    /// The second amplifier.
    AMCurrentAmplifier *amplifier2_;
    /// Bool indicating whether the composite view is valid--if both amplifiers entered are valid.
    bool isValid_;

};

#endif // AMCURRENTAMPLIFIERCOMPOSITEVIEW_H
