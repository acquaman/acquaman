#ifndef PERIODICTABLEVIEW_H
#define PERIODICTABLEVIEW_H

#include <QtGui/QWidget>

#include <QSignalMapper>
#include <QToolButton>
#include <QMouseEvent>

#include "util/AMElement.h"
#include "util/AMPeriodicTable.h"
#include <QDebug>

class AMPeriodicTableView : public QWidget
{
	/*! This function builds a view for the periodic table made to look like the stereotypical view used by every scientist.  When an element is clicked, it emits a signal containing the pointer to that element.
	  */
	Q_OBJECT

public:
	/// Builds the periodic table view using the information contained in periodic table.  It sets up the signals such that clicking on a particular element will emit a signal with the element contained within.
	AMPeriodicTableView(QWidget *parent = 0);
	~AMPeriodicTableView();

	/// Returns the copy of the periodic table used by the view.
	AMPeriodicTable *table() const { return table_; }

signals:
	/// Mapped signal that passes the atomic number of an element.
	void clicked(int);
	/// When an element is clicked on, this signal will be emitted carrying a pointer to the element.
	void elementSelected(AMElement *);

private slots:
	/// Slot that emits a signal carrying a pointer to the particular Element.
	void showElement(int number);

private:
	/// This is a convenience function that takes an Element and returns a mapped QToolButton where the clicked signal is mapped to the element.  Must be called after elementMapper_ has been new'ed.
	QToolButton *mapElement(AMElement *element)
	{
		QToolButton *button = new QToolButton;
		button->setFont(QFont("Times New Roman", 14));
		button->setText(element->symbol());
		button->setFixedSize(35, 25);
		connect(button, SIGNAL(clicked()), elementMapper_, SLOT(map()));
		elementMapper_->setMapping(button, element->atomicNumber().toInt());
		return button;
	}

	// Member variables.
	// The signal mapper.
	QSignalMapper *elementMapper_;
	// The table.
	AMPeriodicTable *table_;
};

#endif // PERIODICTABLEVIEW_H
