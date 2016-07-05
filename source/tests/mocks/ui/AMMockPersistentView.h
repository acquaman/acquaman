#ifndef AMMOCKPERSISTENTVIEW_H
#define AMMOCKPERSISTENTVIEW_H

#include <QWidget>

class AMControlSet;
/*!
  * A ui class which forms the persistant right hand widget of the AMMock
  * application's main window.
  */
class AMMockPersistentView : public QWidget
{
    Q_OBJECT
public:
	/// Creates an instance of an AMMockPErsistentView, with control editors for
	/// each of the controls in the provided control set.
    explicit AMMockPersistentView(AMControlSet* controls,
	                              QWidget *parent = 0);

signals:

public slots:

protected:
	/// Initializes the ui elements and performs layout.
	void setupUi(AMControlSet* controls);
};

#endif // AMMOCKPERSISTENTVIEW_H
