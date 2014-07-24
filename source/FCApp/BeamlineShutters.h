#ifndef BEAMLINESHUTTERS_H
#define BEAMLINESHUTTERS_H

#include <QObject>
#include <QString>


class BeamlineShutters : public QObject
{
    Q_OBJECT
public:
    explicit BeamlineShutters(const QString &name,
                              const QString &psh1,
                              const QString &psh2,
                              const QString &ssh1,
                              QObject *parent = 0);


protected:
    QString nameOfBeamline_;
    QString pvPSH1_;
    QString pvPSH2_;
    QString pvSSH1_;

signals:

public slots:

};

#endif // BEAMLINESHUTTERS_H
