#ifndef PGMPERSISTENTVIEW_H
#define PGMPERSISTENTVIEW_H

#include <QWidget>

class QLabel;
class QGroupBox;
class PGMBeamStatusView;
class PGMBPMStatusView;

class PGMPersistentView : public QWidget
{
    Q_OBJECT
public:
    explicit PGMPersistentView(QWidget *parent = 0);

    /// Contains beamline current lifetime information
    PGMBeamStatusView *beamStatusView_;
    /// Contains the beam position monitor information
    PGMBPMStatusView *bpmStatusView_;


signals:

public slots:

protected slots:

protected:


};

#endif // PGMPERSISTENTVIEW_H
