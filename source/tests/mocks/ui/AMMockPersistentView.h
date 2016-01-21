#ifndef AMMOCKPERSISTENTVIEW_H
#define AMMOCKPERSISTENTVIEW_H

#include <QWidget>

class AMControlSet;
class AMMockPersistentView : public QWidget
{
    Q_OBJECT
public:
    explicit AMMockPersistentView(AMControlSet* controls,
	                              QWidget *parent = 0);

signals:

public slots:

protected:
	void setupUi(AMControlSet* controls);
};

#endif // AMMOCKPERSISTENTVIEW_H
