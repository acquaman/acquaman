#ifndef AMCUSTOMIZABLEPERIODICTABLE_H
#define AMCUSTOMIZABLEPERIODICTABLE_H

#include <QObject>
#include <QList>

#include "AMElement.h"

#define AMCUSTOMIZABLEPERIODICTABLE_COULD_NOT_LOAD_ELEMENTAL_INFORMATION 323000

/// This class is a container for AMElements in a similar manner as AMPeriodicTable.  The primary difference being that this periodic table is setup to enable subclassing when such behaviour is desirable.
class AMCustomizablePeriodicTable : public QObject
{
	Q_OBJECT

public:
	/// Constructor.  Builds the periodic table from a file and creates a list of elements.
 	virtual ~AMCustomizablePeriodicTable();
	explicit AMCustomizablePeriodicTable(QObject *parent = 0);

	/// Builds the periodic table model.  Must be called after the table has been created.  Subclasses should reimplement if more sophisticated subclasses of AMElement are required.
	virtual void buildPeriodicTable();

	/// Returns the list of elements.
	QList<AMElement *> elements() const { return periodicTable_; }
	/// Returns the number of elements in the periodic table.
	int numberOfElements() const { return periodicTable_.size(); }

	/// Returns the element specified by the given \em name.  Returns 0 if \em name doesn't exist.
	AMElement *elementByName(QString name) const;
	/// Returns the element specified by the given \em symbol.  Returns 0 if \em symbol doesn't exist.
	AMElement *elementBySymbol(QString symbol) const;
	/// Returns the element specified by the given atomic number.  The number must be a valid atomic number between 1 <= atomicNumber <= 109.
	AMElement *elementByAtomicNumber(int number) const { return periodicTable_.at(number-1); }

signals:

public slots:

protected:
	/// The periodic table.  In all its glory.  Stores all AMElement *'s in a list.
	QList<AMElement *> periodicTable_;
};

#endif // AMCUSTOMIZABLEPERIODICTABLE_H
