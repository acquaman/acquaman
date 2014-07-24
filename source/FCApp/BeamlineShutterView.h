#ifndef BEAMLINESHUTTERVIEW_H
#define BEAMLINESHUTTERVIEW_H

#include <QObject>
#include "BeamlineShutters.h"

// View Class for BeamlineShutter Model
// The constructor takes in a model as a parameter and then something happens.

class BeamlineShutterView : public QObject
{
    Q_OBJECT
public:
    explicit BeamlineShutterView(BeamlineShutters *beamline, QObject *parent = 0);


signals:


public slots:
    void updatePSH1Pixmap(int value);
    void updatePSH2Pixmap(int value);
    void updateSSH1Pixmap(int value);

protected:
    QLabel *name_;
    QLabel *psh1Light_;
    QLabel *psh2Light_;
    QLabel *ssh1Light_;

    BeamlineShutters *model_;

protected slots:


};

#endif // BEAMLINESHUTTERVIEW_H
