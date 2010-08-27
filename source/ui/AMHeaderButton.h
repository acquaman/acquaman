#ifndef AMHEADERBUTTON_H
#define AMHEADERBUTTON_H

#include <QToolButton>
#include <QMetaType>

/// This class provides a styled QToolButton with a look and feel appropriate for a title strip across the tops of other widgets. The button's clickability is used by AMVerticalStackWidget to trigger hiding and unhiding of the sub-widgets.
class AMHeaderButton : public QToolButton
{
Q_OBJECT
public:
	explicit AMHeaderButton(QWidget *parent = 0);


	void setArrowType(Qt::ArrowType type);

	Qt::ArrowType arrowType() const { return arrowType_; }

signals:

public slots:

protected:
	Qt::ArrowType arrowType_;

	/// re-implemented from QToolButton to display arrows way over on the right, and keep the name shown
	virtual void paintEvent(QPaintEvent *);

	QPixmap arrowPix;


};

Q_DECLARE_METATYPE(AMHeaderButton*);

#endif // AMHEADERBUTTON_H
