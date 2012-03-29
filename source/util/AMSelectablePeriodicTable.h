#ifndef AMSELECTABLEPERIODICTABLE_H
#define AMSELECTABLEPERIODICTABLE_H

#include <QObject>

///	This class encapsulates the periodic table and extends it's functionality slightly by offering a "memory" of selected elements.
class AMSelectablePeriodicTable : public QObject
{
	Q_OBJECT
public:
	/// Constructor.
	explicit AMSelectablePeriodicTable(QObject *parent = 0);

signals:

public slots:

protected:
	/// List of atomic numbers that have been saved.
	QList<int> savedElements_;
};

#endif // AMSELECTABLEPERIODICTABLE_H
