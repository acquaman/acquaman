#ifndef CLSCONTROL_H
#define CLSCONTROL_H

#include "beamline/AMControl.h"

class CLSControl : public AMControl
{
    Q_OBJECT

public:
	/// Constructor.
	explicit CLSControl(const QString &name, const QString &units, QObject *parent = 0, const QString &description = QString());
	/// Destructor.
	virtual ~CLSControl();

	/// Returns true if this control is an enumerated control, with descrete value options.
	virtual bool isEnum() const { return !enumIndexStringMap_.keys().isEmpty(); }

	/// Returns the string representation of the given enum index. Returns an empty string if the given index is not a valid enum index.
	virtual QString enumIndexName(double index) const { return enumIndexStringMap_.value(index, QString()); }
	/// Returns the enum index that corresponds to the given index name. Returns -1 if the string does not correspond to a valid enum index.
	virtual double enumIndexNamed(const QString &name) const;

	/// Returns true if the given value is a valid enum index.
	virtual bool hasEnumIndex(double value) const { return enumIndexStringMap_.keys().contains(value); }
	/// Returns true if there is an existing enum option index with the given name.
	virtual bool hasEnumIndexNamed(const QString &name) const { return (enumIndexNamed(name) != -1); }

	/// Returns the list of enum indices.
	virtual QList<double> enumIndices() const { return enumIndexStringMap_.keys(); }
	/// Returns the list of read-only enum indices.
	virtual QList<double> readOnlyEnumIndices() const { return readOnlyEnumIndices_; }
	/// Returns the list of move enum indices.
	virtual QList<double> moveEnumIndices() const;

	/// Returns the list of names for all enum indices.
	virtual QStringList enumIndexNames() const { return enumIndexStringMap_.values(); }
	/// Returns the list of names for read-only enum indices.
	virtual QStringList readOnlyEnumNames() const;
	/// Returns the list of names for move enum indices.
	virtual QStringList moveEnumNames() const;

signals:

public slots:

protected slots:
	/// Adds a read-only enum value option. Options added with duplicate indices will overwrite previous entries.
	virtual bool addReadOnlyEnumOption(int index, const QString &optionString);
	/// Adds an enum value option. Options added with duplicate indices will overwrite previous options.
	virtual bool addEnumOption(int index, const QString &optionString);
	/// Removes an enum value option.
	virtual bool removeEnumOption(int index);
	/// Clears all value options.
	virtual bool clearEnumOptions();

private:
	/// The list of enum indices for read-only enum states (eg. Invalid, Unknown, Between, etc...).
	QList<double> readOnlyEnumIndices_;
	/// The mapping between enum index and string representation.
	QMap<double, QString> enumIndexStringMap_;
};

#endif // CLSCONTROL_H
