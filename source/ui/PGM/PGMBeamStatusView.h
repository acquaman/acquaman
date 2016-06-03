#ifndef PGMBEAMSTATUSVIEW_H
#define PGMBEAMSTATUSVIEW_H

#include <QWidget>

class QLabel;


/// This class is meant to display common beam status information that would
/// be shared across both branches at VLS-PGM. Should keep PGMPersistentView less
/// cluttered.


class PGMBeamStatusView : public QWidget
{
    Q_OBJECT
public:
    explicit PGMBeamStatusView(QWidget *parent = 0);

signals:

public slots:

protected slots:
    /// Handles updating change in storage ring current.
    void onRingCurrentChange(double value);
    /// handles updating lifetime value
    void onLifetimeChange(double value);

protected:
    /// Storage ring current lable and value.
    QLabel *ringCurrentValue_;
    QLabel *ringCurrentLabel_;
    /// Beam lifetime lable and value.
    QLabel *beamLifetimeValue_;
    QLabel *beamLifetimeLabel_;

};

#endif // PGMBEAMSTATUSVIEW_H
