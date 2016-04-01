#ifndef PGMPERSISTENTVIEW_H
#define PGMPERSISTENTVIEW_H

#include <QWidget>

class AMExtendedControlEditor;

class PGMPersistentView : public QWidget
{
    Q_OBJECT
public:
    explicit PGMPersistentView(QWidget *parent = 0);

signals:

public slots:

protected:
    AMExtendedControlEditor *entranceSlitGapControlEditor_;


};

#endif // PGMPERSISTENTVIEW_H
