#ifndef BIOXASSIDEPERSISTENTVIEW_H
#define BIOXASSIDEPERSISTENTVIEW_H

#include <QWidget>
#include <QLayout>

#include "beamline/CLS/CLSKeithley428.h"
#include "ui/CLS/CLSKeithley428View.h"

class AMExtendedControlEditor;
class CLSKeithley428ContainerView;

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
    CLSKeithley428View *keithleyView_;

};

#endif // BIOXASSIDEPERSISTENTVIEW_H
