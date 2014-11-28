#ifndef WATERVALVE_H
#define WATERVALVE_H

#include <QObject>
#include <QString>
#include "beamline/AMProcessVariable.h"

class WaterValve : public QObject
{
    Q_OBJECT
public:
    explicit WaterValve(const QString &name,
                                            const QList<QString> &s_pvlist,
                                            QObject *parent = 0);


 QList<int> StatusList() const { return statuslist_; }
 QString NameOfBeamline() const { return nameOfBeamline_; }


signals:
    void valveStatusChanged(bool value);

public slots:

protected slots:
    void updateValveStatus();


protected:
    //The pvlist updates the statuslist_ which is what gets passed to the view function
    QList<AMProcessVariable* > pvList_;
    QList<int> statuslist_;
    QString nameOfBeamline_;


};

#endif // WATERVALVE_H
