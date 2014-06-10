#ifndef AMNAMEANDRANGEVALIDATOR_H
#define AMNAMEANDRANGEVALIDATOR_H

#include <QObject>

#include <QString>
#include <QStringList>

#include "util/AMRange.h"

/// This class handles validating the name from a list of filters and value within an AMRange.
class AMNameAndRangeValidator : public QObject
{
	Q_OBJECT

public:
	/// Builds a default validator.  All results are valid with no constraints to compare against.
 	virtual ~AMNameAndRangeValidator();
	explicit AMNameAndRangeValidator(QObject *parent = 0);
	/// Builds a validator with the provided range.
	AMNameAndRangeValidator(const AMRange &range, QObject *parent = 0);
	/// Builds a validator with the provided list of name filters.
	AMNameAndRangeValidator(const QStringList &filters, QObject *parent = 0);
	/// Builds a validator with the provided list of name filters.
	AMNameAndRangeValidator(const QList<QRegExp> &filters, QObject *parent = 0);
	/// Builds a validator with the provided list of name filters and provided range.
	AMNameAndRangeValidator(const QStringList &filters, const AMRange &range, QObject *parent = 0);
	/// Builds a validator with the provided list of name filters and provided range.
	AMNameAndRangeValidator(const QList<QRegExp> &filters, const AMRange &range, QObject *parent = 0);

	/// Returns whether the string is valid.
	bool isValid(const QString &name) const;
	/// Overloaded.  Returns whether the valie is within the range.
	bool isValid(double value) const;
	/// Overloaded.  Returns whether the string and value is valid.
	bool isValid(const QString &name, double value) const;

	/// Returns the energy range used for filtering.  If no range has been provided the range is null.
	const AMRange &range() const { return range_; }
	/// Returns the minimum energy in the energy range filter.
	double minimum() const { return range_.minimum(); }
	/// Returns the maximum energy in the energy range filter.
	double maximum() const { return range_.maximum(); }

	/// Returns the list of name filters.
	const QList<QRegExp> &nameFilters() const { return nameFilters_; }
	/// Returns the name filter at a given index.
	const QRegExp &nameFilterAt(int index) const { return nameFilters_.at(index); }
	/// Removes the name filter at the given index.  Returns whether the removal was successful.
	bool removeNameFilter(int index);
	/// Removes the name filter with the given name filter.  Returns whether the removal was successful.
	bool removeNameFilter(const QRegExp &filter);
	/// Removes the name filter with the given name filter.  Returns whether the removal was successful.
	bool removeNameFilter(const QString &filter);
	/// Adds a new name filter to the list of name filters.
	void addNameFilter(const QRegExp &newNameFilter);
	/// Adds a new name filter to the list of name filters.
	void addNameFilter(const QString &newNameFilter);

signals:
	/// Notifier that the contents of the validator have changed.  This is emitted after any changes to the name filters list or the range is modified.
	void validatorChanged();

public slots:
	/// Sets the range filter for this view.
	void setRange(const AMRange &newRange);
	/// Sets the range filter for this view.
	void setRange(double minimum, double maximum);
	/// Sets a new minimum value for the range.
	void setMinimum(double newMinimum);
	/// Sets a new maximum value for the range.
	void setMaximum(double newMaximum);

protected:
	/// The value range filter.
	AMRange range_;
	/// The list of name filters.
	QList<QRegExp> nameFilters_;
};

#endif // AMNAMEANDRANGEVALIDATOR_H
