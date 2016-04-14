#ifndef PGMPERSISTENTVIEW_H
#define PGMPERSISTENTVIEW_H

#include <QWidget>

class AMExtendedControlEditor;
class QLabel;

class PGMPersistentView : public QWidget
{
    Q_OBJECT
public:
    explicit PGMPersistentView(QWidget *parent = 0);

signals:

public slots:

protected slots:
    /// Handles current change in upper blade - branch A
    void onUpperBladeCurrentAChanged(double value);
    /// Handles current change in lower blade - branch A
    void onLowerBladeCurrentAChanged(double value);
    /// Handles current change in upper blade - branch B
    void onUpperBladeCurrentBChanged(double value);
    /// Handles current change in lower blade - branch B
    void onLowerBladeCurrentBChanged(double value);


protected:
    /// Control editors for exit slit gap and position
    AMExtendedControlEditor *exitSlitGapBranchAControlEditor_;
    AMExtendedControlEditor *exitSlitGapBranchBControlEditor_;
    AMExtendedControlEditor *exitPositionBranchAControlEditor_;
    AMExtendedControlEditor *exitPositionBranchBControlEditor_;


};

#endif // PGMPERSISTENTVIEW_H
