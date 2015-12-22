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

	/// Returns true if this control is connected.
	virtual bool isConnected() const { return connected_; }

	/// Returns true if this control is an enumerated control (cached).
	virtual bool isEnum() const { return enumerated_; }

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

public slots:

protected slots:
	/// Sets the connected state.
	void setConnected(bool isConnected);
	/// Updates the connected state.
	virtual void updateConnected();

	/// Sets the enumerated state.
	void setEnumerated(bool isEnumerated);
	/// Updates the enumerated state.
	virtual void updateEnumerated();

	/// Adds a read-only enum value option. Options added with duplicate indices will overwrite previous entries.
	bool addReadOnlyEnumOption(int index, const QString &optionString);
	/// Adds an enum value option. Options added with duplicate indices will overwrite previous options.
	bool addEnumOption(int index, const QString &optionString);
	/// Removes an enum value option.
	bool removeEnumOption(int index);
	/// Clears all value options.
	bool clearEnumOptions();

protected:
	/// Returns the current connected state.
	virtual bool currentConnected() const { return false; }
	/// Returns true if this control is an enumerated control, with descrete value options.
	virtual bool currentEnumerated() const { return !enumIndices().isEmpty(); }

private:
	/// The (cached) connected state.
	bool connected_;
	/// The (cached) enumeration state.
	bool enumerated_;
	/// The list of enum indices for read-only enum indices.
	QList<double> readOnlyEnumIndices_;
	/// The mapping between enum index and string representation.
	QMap<double, QString> enumIndexStringMap_;
};

#endif // CLSCONTROL_H
