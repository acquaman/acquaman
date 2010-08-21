#ifndef AMCONTROLSETINFO_H
#define AMCONTROLSETINFO_H

#include "dataman/AMDbObject.h"
#include "dataman/AMDatabaseDefinition.h"
#include <QStringList>

class AMControlSetInfo : public AMDbObject
{
Q_OBJECT
public:
	explicit AMControlSetInfo(QObject *parent = 0);
	AMControlSetInfo(AMControlSetInfo *copyFrom, QObject *parent = 0);

	~AMControlSetInfo() {}

	int count() const;

	QString nameAt(int index) const;

	double valueAt(int index) const;

	double minimumAt(int index) const;

	double maximumAt(int index) const;

	// AMDbObject database interface
	////////////////////////////////////
	/// Specify all of our unique pieces of meta-data (excluding those inherited from base classes -- ie: own only)
	static QList<AMMetaMetaData> metaDataUniqueKeys();

	/// This function needs to be overloaded to return all the available pieces of meta data for this type of object, including those inherited from base classes. (ie: own + base classes'). We simply append our unique meta-data onto the base class:
	static QList<AMMetaMetaData> metaDataKeys();

	/// This virtual function returns all the available pieces of meta data for this type of object, by introspecting it's most detailed type. (ie: own + base classes' + subclasses')
	virtual QList<AMMetaMetaData> metaDataAllKeys() const;

	/// We want to store this in a separate table (so that it's easy to create relationships between detectors and scan objects).  Therefore, we reimplement databaseTableName():
	virtual QString databaseTableName() const;

	/// Reimplemented from AMDbObject; provides a general human-readable description
	virtual QString typeDescription() const;

signals:
	void valuesChanged();

public slots:
	/// Convenience function to set the human-readable description
	void setDescription(const QString& description);

	bool setNameAt(int index, QString name);

	bool setValueAt(int index, double value);

	bool setMinimumAt(int index, double minimum);

	bool setMaximumAt(int index, double maximum);

	bool setControlAt(int index, QString name, double value, double minimum, double maximum);

	bool addControlAt(int index, QString name, double value, double minimum, double maximum);

	bool removeControlAt(int index);

	void copyFrom(AMControlSetInfo *copyFrom);
};

#endif // AMCONTROLSETINFO_H
