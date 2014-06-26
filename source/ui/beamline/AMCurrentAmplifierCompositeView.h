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

signals:

public slots:

protected slots:
    virtual void onValueComboBoxChanged(const QString &newText);
    virtual void onMinusClicked();
    virtual void onPlusClicked();

protected:
    /// The first amplifier.
    AMCurrentAmplifier *amplifier1_;
    /// The second amplifier.
    AMCurrentAmplifier *amplifier2_;

};

#endif // AMCURRENTAMPLIFIERCOMPOSITEVIEW_H
