#ifndef SGMPERIODICTABLE_H
#define SGMPERIODICTABLE_H

#include "util/AMPeriodicTable.h"
#include "util/SGM/SGMElementInfo.h"

class SGMPeriodicTable : public QObject
{
Q_OBJECT
public:
	/// Access to the application-wide SGM Periodic table object.
	static SGMPeriodicTable* sgmTable(){
		if(!instance_)
			instance_ = new SGMPeriodicTable();
		return instance_;
	}

	SGMElementInfo* elementInfoByName(const QString &elementName) const;
	QStringList fastScanPresetsStrings() const;
	QList<SGMFastScanParameters*> fastScanPresets() const;


protected:
	/// Holds the additional SGM Information for elements
	AMOrderedSet<AMElement*, SGMElementInfo*> sgmPeriodicTableInfo_;

	/// Singleton instance variable
	static SGMPeriodicTable *instance_;

	/// Constructor: Builds the SGM Element info attached to the periodic table (protected: access via sgmTable())
	explicit SGMPeriodicTable(QObject *parent = 0);
};

#endif // SGMPERIODICTABLE_H
