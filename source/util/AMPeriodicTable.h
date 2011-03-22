#ifndef PERIODICTABLE_H
#define PERIODICTABLE_H

#include <QObject>
#include <QList>

#include "AMElement.h"

class AMPeriodicTable : public QObject
{
	/*! This class is a container for AMElements that provides the convenience of retrieving an element in an intuitive way.
	  */
	Q_OBJECT
public:
	/// Builds the periodic table from a file and creates a list of elements.
	explicit AMPeriodicTable(QObject *parent = 0);

	/// Returns the table of elements.
	QList<AMElement *> periodicTable() const { return periodicTable_; }

	/// Returns the element specified by the given \em name.  Returns 0 if \em name doesn't exist.
	AMElement *elementByName(QString name) const
	{
		for (int i = 0; i < periodicTable_.size(); i++)
			if (QString::compare(periodicTable_.at(i)->name(), name) == 0)
				return periodicTable_.at(i);

		return 0;
	}

	/// Returns the element specified by the given \em symbol.  Returns 0 if \em symbol doesn't exist.
	AMElement *elementBySymbol(QString symbol) const
	{
		for (int i = 0; i < periodicTable_.size(); i++)
			if (QString::compare(periodicTable_.at(i)->symbol(), symbol) == 0)
				return periodicTable_.at(i);

		return 0;
	}

	/// Returns the element specified by the given atomic number.  The number must be a valid atomic number between 1 <= atomicNumber <= 109.
	AMElement *elementByNumber(int number) const { return periodicTable_.at(number-1); }

	/// Overloaded function.  If given a string instead of a integer.
	AMElement *elementByNumber(QString number) const { return elementByNumber(number.toInt()); }

signals:
	/// Signal emitted if the the periodic table fails to load.  The parameters passed are the file name and error message.
	void unableToLoad(QString, QString);

public slots:

private:
	// Member variables.
	// The periodic table.  In all its glory.
	QList<AMElement *> periodicTable_;

};

#endif // PERIODICTABLE_H
