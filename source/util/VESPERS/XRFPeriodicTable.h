#ifndef XRFPERIODICTABLE_H
#define XRFPERIODICTABLE_H

#include <QObject>

#include "util/AMElement.h"
#include "util/AMPeriodicTable.h"

class XRFPeriodicTable : public QObject
{
	Q_OBJECT
public:
	/// Default constructor.
	explicit XRFPeriodicTable(QObject *parent = 0);

	/// Returns the list of elements.  \bug Should this be const? Right now it gives read-write access to the elements that are shared with the original table, via the AMElement pointers.
	QList<AMElement *> elements() const { return AMPeriodicTable::table()->elements(); }

	/// Returns the element specified by the given \em name.  Returns 0 if \em name doesn't exist.
	AMElement *elementByName(QString name) const { return AMPeriodicTable::table()->elementByName(name); }

	/// Returns the element specified by the given \em symbol.  Returns 0 if \em symbol doesn't exist.
	AMElement *elementBySymbol(QString symbol) const { return AMPeriodicTable::table()->elementBySymbol(symbol); }

	/// Returns the element specified by the given atomic number.  The number must be a valid atomic number between 1 <= atomicNumber <= 109.
	AMElement *elementByAtomicNumber(int number) const { return AMPeriodicTable::table()->elementByAtomicNumber(number); }

	/// Returns the list of checked elements.
	QList<QPair<int, QString> > checkedList() const { return checkedList_; }

	/// Adds an element/line combo to the checked list, if it already doesn't reside there.
	bool addToList(AMElement *el, QPair<QString, QString> line);
	/// Removes an element/line combo from the checked list, if it exists in the list.
	bool removeFromList(AMElement *el, QPair<QString, QString> line);
	/// Clears the list.
	void clearList() { checkedList_.clear(); }

signals:

public slots:

protected:

	/// Holds a list with the atomic number paired with an emission line name.
	QList<QPair<int, QString> > checkedList_;

};

#endif // XRFPERIODICTABLE_H
