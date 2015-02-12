#ifndef AMPENSTYLECOMBOBOX_H
#define AMPENSTYLECOMBOBOX_H

#include <QComboBox>
#include <QAbstractItemView>
#include <QPixmap>
#include <QPainter>
#include <QStylePainter>
#include <QPaintEvent>
#include <QItemDelegate>
/**
 * Combo Box widget which displays different pen styles for the user to
 * select from
 */
class AMPenStyleComboBox : public QComboBox
{
	Q_OBJECT
public:
	/// Creates an instance of an AMPenStyleComboBox, which will display all
	/// the pen styles currently in Qt4.8 - from Qt::SolidLine on.
	explicit AMPenStyleComboBox(QWidget *parent = 0);
	
	/// The currently selected Pen Style
	Qt::PenStyle selectedPenStyle() const;
	/// Sets the currently selected Pen Style
	void setSelectedPenStyle(Qt::PenStyle penStyle);
signals:
	/// Emmitted when the selected pen style changes
	void currentPenStyleChanged(Qt::PenStyle newStyle);
public slots:
protected:
	void paintEvent(QPaintEvent*);
protected slots:
	/// Internally handles the handling of the index changes. Means that the
	/// changes can be listened to in terms of PenStyle, rather than index.
	void onCurrentIndexChanged(int);
	
};

/**
 * Delegate class used for drawing the drop down items of AMPenStyleComboBox
 *	NOTE: Qt for some reason has the delegate only responsible for the items
 *		in the actual popup menu. Responsibility for the drawing for the actual
 *		widget part, and the currently selected its are handled by
 *		AMPenStyleComboBox::paintEvent(QPaintEvent)
 */
class AMPenStyleDelegate : public QItemDelegate
{
public:
	/// Creates an instance of an AMPenStyleDelegate
	explicit AMPenStyleDelegate(QObject* parent = 0);

protected:
	/// Paints the individual items in the drop down list of the AMPenStyleComboBox
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // AMPENSTYLECOMBOBOX_H
