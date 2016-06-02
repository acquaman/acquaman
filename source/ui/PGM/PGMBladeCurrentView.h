#ifndef PGMBLADECURRENTVIEW_H
#define PGMBLADECURRENTVIEW_H

#include <QWidget>

class QLabel;
class QGroupBox;

/// This view class display common blade current information for both
/// branches. Contains blade currents for both exit slits, entrance slit, TEY,
/// FLY, I0 Beamline and I0 Endstation.
class PGMBladeCurrentView : public QWidget
{
    Q_OBJECT
public:
    /// Constructor.
    explicit PGMBladeCurrentView(QWidget *parent = 0);

signals:

public slots:
};

#endif // PGMBLADECURRENTVIEW_H
