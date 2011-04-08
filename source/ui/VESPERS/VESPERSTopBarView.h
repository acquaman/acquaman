#ifndef VESPERSTOPBARVIEW_H
#define VESPERSTOPBARVIEW_H

#include <QWidget>

/*! This class is the top bar of the user interface.  This is a persistent widget that will always be visible regardless of what is going
  on inside the program.  Essential things will be displayed here and currently holds:

	- safety shutter status and buttons

  */
class VESPERSTopBarView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.
	explicit VESPERSTopBarView(QWidget *parent = 0);

signals:

public slots:

};

#endif // VESPERSTOPBARVIEW_H
