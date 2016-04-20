#ifndef PGMPERSISTENTVIEW_H
#define PGMPERSISTENTVIEW_H

#include <QWidget>

class QLabel;
class QGroupBox;
class PGMBladeCurrentView;

class PGMPersistentView : public QWidget
{
    Q_OBJECT
public:
    explicit PGMPersistentView(QWidget *parent = 0);

signals:

public slots:

protected slots:

protected:
    /// View containing blade current information for both branches.
    PGMBladeCurrentView *bladeCurrentView_;



};

#endif // PGMPERSISTENTVIEW_H
