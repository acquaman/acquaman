#ifndef BIOXASSIDEPERSISTENTVIEW_H
#define BIOXASSIDEPERSISTENTVIEW_H

#include <QWidget>
#include <QLayout>

#include "beamline/CLS/CLSKeithley428.h"
#include "ui/beamline/AMCurrentAmplifierSingleView.h"

class AMExtendedControlEditor;
class AMCurrentAmplifierView;

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

};

#endif // BIOXASSIDEPERSISTENTVIEW_H
