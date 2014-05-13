#ifndef BIOXASSIDEPERSISTENTVIEW_H
#define BIOXASSIDEPERSISTENTVIEW_H

#include <QWidget>
#include <QLayout>

class AMExtendedControlEditor;

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

};

#endif // BIOXASSIDEPERSISTENTVIEW_H
