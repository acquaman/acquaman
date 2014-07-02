#ifndef BIOXASSIDEPERSISTENTVIEW_H
#define BIOXASSIDEPERSISTENTVIEW_H

#include <QWidget>
#include <QLayout>

#include "beamline/CLS/CLSMAXvMotor.h"
#include "beamline/CLS/CLSKeithley428.h"
#include "ui/beamline/AMCurrentAmplifierSingleView.h"
#include "ui/beamline/AMCurrentAmplifierCompositeView.h"

class AMExtendedControlEditor;

class BioXASSidePersistentView : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASSidePersistentView(QWidget *parent = 0);

signals:

public slots:

protected:
    AMExtendedControlEditor *motorControlEditor_;
    AMCurrentAmplifierSingleView *keithleyView_;
    AMCurrentAmplifierCompositeView *testView_;

};

#endif // BIOXASSIDEPERSISTENTVIEW_H
