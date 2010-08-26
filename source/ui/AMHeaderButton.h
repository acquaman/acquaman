#ifndef AMHEADERBUTTON_H
#define AMHEADERBUTTON_H

#include <QToolButton>

/// This class provides a styled QToolButton with a look and feel appropriate for a title strip across the tops of other widgets. The button's clickability is used by AMVerticalStackWidget to trigger hiding and unhiding of the sub-widgets.
class AMHeaderButton : public QToolButton
{
Q_OBJECT
public:
	explicit AMHeaderButton(QWidget *parent = 0);

signals:

public slots:

};

#endif // AMHEADERBUTTON_H
