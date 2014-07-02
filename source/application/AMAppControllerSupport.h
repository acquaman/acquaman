/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMAPPCONTROLLERSUPPORT_H
#define AMAPPCONTROLLERSUPPORT_H

#include <QString>
#include <QHash>

#include "dataman/database/AMDatabase.h"

//class QMetaObject;

class AMScanConfiguration;
class AMExporter;
class AMExporterOption;

class AMScanActionControllerScanOptimizer;
class AMScanActionControllerScanValidator;
class AMAction3;

class AMActionRunner3;
class AMActionHistoryModel3;

class AMScanConfigurationObjectInfo{
public:
	/// The default constructor creates an invalid object
 	virtual ~AMScanConfigurationObjectInfo();
	AMScanConfigurationObjectInfo(){
		scanConfigurationMetaObject = 0;
		exporterMetaObject = 0;
		exporterOptionMetaObject = 0;
		exporterOptionId = 0;
		databaseName = QString();
	}

	/// fill the className
	AMScanConfigurationObjectInfo(AMScanConfiguration *prototypeScanConfiguration, AMExporter *prototypeExporter, AMExporterOption *prototypeExporterOption, int useExporterOptionId, QString useDatabaseName = QString());

	/// fill the className (This version doesn't require an instance. The \c classMetaObject can be retrieved statically with Class::staticMetaObject. )
	AMScanConfigurationObjectInfo(const QMetaObject *useScanConfigurationMetaObject, const QMetaObject *useExporterMetaObject, const QMetaObject *useExporterOptionMetaObject, int useExporterOptionId, QString useDatabaseName = QString());

	/// Indicates this AMDbObjectInfo represents a valid object.
	bool isValid() const {
		return ( (scanConfigurationMetaObject != 0) && (exporterMetaObject != 0) && (exporterOptionMetaObject != 0) );
	}

	QString scanConfigurationClassName; ///< the class name (C++ type name) of the scan configuration object
	QString exporterClassName; ///< the class name (C++ type name) of the exporter object
	QString exporterOptionClassName; ///< the class name (C++ type name) of the exporter optoin object
	const QMetaObject *scanConfigurationMetaObject; ///< QMetaObject pointer with the complete meta-object for the scan configuration
	const QMetaObject *exporterMetaObject; ///< QMetaObject pointer with the complete meta-object for the exporter
	const QMetaObject *exporterOptionMetaObject; ///< QMetaObject pointer with the complete meta-object for the exporter option
	int exporterOptionId; ///< Database Id for the exporter option you want to use (right now assumes the userDb)
	QString databaseName; ///< Database name where the exporter option is stored (an empty string implies "user" database)

private:
	/// used to implement both constructors
	void initWithMetaObject(const QMetaObject *useScanConfigurationMetaObject, const QMetaObject *useExporterMetaObject, const QMetaObject *useExporterOptionMetaObject, int useExporterOptionId, QString useDatabaseName = QString());

	/// checks to make sure a QMetaObject inherits AMScanConfiguration
	bool inheritsScanConfiguration(const QMetaObject *metaObject) const;

	/// checks to make sure a QMetaObject inherits AMExporter
	bool inheritsExporter(const QMetaObject *metaObject) const;

	/// checks to make sure a QMetaObject inherits AMExporterOption
	bool inheritsExporterOption(const QMetaObject *metaObject) const;
};

class AMActionRunnerGroup{
public:
 	virtual ~AMActionRunnerGroup();
	AMActionRunnerGroup(const QString &databaseName, AMActionRunner3 *actionRunner, AMActionHistoryModel3 *actionHistoryModel){
		databaseName_ = databaseName;
		actionRunner_ = actionRunner;
		actionHistoryModel_ = actionHistoryModel;
	}

	QString databaseName() const { return databaseName_; }
	AMActionRunner3* actionRunner() const { return actionRunner_; }
	AMActionHistoryModel3* actionHistoryModel() const { return actionHistoryModel_; }

protected:
	QString databaseName_;
	AMActionRunner3 *actionRunner_;
	AMActionHistoryModel3 *actionHistoryModel_;
};

namespace AMAppControllerSupport{
	extern QHash<QString, AMScanConfigurationObjectInfo> registeredClasses_;

	// Registers the triplet of a scan configuration class (AMScanConfiguration descendant), an exporter class (AMExporter descendant), and an exporter option class (AMExporterOption descendent)
	// Class T1 needs to inherit AMScanConfiguration, Class T3 needs to inherit AMExporter, and Class T3 needs to inherit AMExporterOption
	template <class Ta, class Tb, class Tc>
	bool registerClass(int exporterOptionId, QString databaseName = QString()) {
		// make sure this is a valid database id
		if( exporterOptionId < 1)
			return false;

		if(!databaseName.isEmpty()){
			AMDatabase *useDatabase = AMDatabase::database(databaseName);
			if(!useDatabase)
				return false;
		}

		// create the meta object for the scan configuration
		const QMetaObject *scanConfigurationMo = &(Ta::staticMetaObject);
		// create the meta object for the exporter
		const QMetaObject *exporterMo = &(Tb::staticMetaObject);
		// create the meta object for the exporter option
		const QMetaObject *exporterOptionMo = &(Tc::staticMetaObject);

		// is this a subclass of AMScanConfiguration? (Or an AMScanConfiguration itself?)
		const QMetaObject* scanConfigurationSuperClass = scanConfigurationMo;
		bool inheritsScanConfiguration;
		do {
			inheritsScanConfiguration = (scanConfigurationSuperClass->className() == QString("AMScanConfiguration"));
		}
		while( (scanConfigurationSuperClass=scanConfigurationSuperClass->superClass()) && inheritsScanConfiguration == false );
		// is this a subclass of AMDetectorView? (Or an AMDetectorView itself?)
		if(!inheritsScanConfiguration)
			return false;

		// is this a subclass of AMExporter? (Or an AMExporter itself?)
		const QMetaObject* exporterSuperClass = exporterMo;
		bool inheritsExporter;
		do {
			inheritsExporter = (exporterSuperClass->className() == QString("AMExporter"));
		}
		while( (exporterSuperClass=exporterSuperClass->superClass()) && inheritsExporter == false );
		if(!inheritsExporter)
			return false;

		const QMetaObject* exporterOptionSuperClass = exporterOptionMo;
		bool inheritsExporterOption;
		do {
			inheritsExporterOption = (exporterOptionSuperClass->className() == QString("AMExporterOption"));
		}
		while( (exporterOptionSuperClass=exporterOptionSuperClass->superClass()) && inheritsExporterOption == false );
		if(!inheritsExporterOption)
			return false;

		// is it already registered? return true.
		QString className(scanConfigurationMo->className());
//		if(registeredClasses_.contains(className)) {
//			//AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, 0, QString("Detector View Support: The class '%1' has already been registered with detector view. Skipping duplicate registration.").arg(className)));
//			return true;
//		}

		// Overwriting the old one.
		AMScanConfigurationObjectInfo newInfo(scanConfigurationMo, exporterMo, exporterOptionMo, exporterOptionId, databaseName);

		registeredClasses_.insert(className, newInfo);
		return true;
	}

	/// returns a const pointer to the hash of registered AMScanConfigurationObjectInfo classes
	const QHash<QString, AMScanConfigurationObjectInfo>* registeredClasses();

	/// Useful for auto export, this creates the available AMExporter class for the AMScanConfiguration. You can use qobject_cast<>() or type() to find out the detailed type of the new object.  Returns 0 if no object found.
	/*! Ownership of the newly-created object becomes the responsibility of the caller. */
	AMExporter* createExporter(AMScanConfiguration *scanConfiguration);

	/// Useful for auto export, this creates the available AMExporterOption class for the AMScanConfiguration. You can use qobject_cast<>() or type() to find out the detailed type of the new object.  Returns 0 if no object found.
	/*! Ownership of the newly-created object becomes the responsibility of the caller. */
	AMExporterOption* createExporterOption(AMScanConfiguration *scanConfiguration);

	extern QList<AMScanActionControllerScanOptimizer*> principleOptimizers_;
	extern QList<AMScanActionControllerScanValidator*> principleValidators_;

	int principleOptimizerCount();
	int principleValidatorCount();

	AMScanActionControllerScanOptimizer* principleOptimizerAt(int index);
	AMScanActionControllerScanValidator* principleValidatorAt(int index);

	void addPrincipleOptimizer(int index, AMScanActionControllerScanOptimizer *optimizer);
	void addPrincipleValidator(int index, AMScanActionControllerScanValidator *validator);

	void appendPrincipleOptimizer(AMScanActionControllerScanOptimizer *optimizer);
	void appendPrincipleValidator(AMScanActionControllerScanValidator *validator);

	void prependPrincipleOptimizer(AMScanActionControllerScanOptimizer *optimizer);
	void prependPrincipleValidator(AMScanActionControllerScanValidator *validator);

	AMScanActionControllerScanOptimizer* removePrincipleOptimizer(int index);
	AMScanActionControllerScanValidator* removePrincipleValidator(int index);

	QList<AMScanActionControllerScanOptimizer*> principleOptimizersCopy();
	QList<AMScanActionControllerScanValidator*> principleValidatorsCopy();

	void optimize(QList<AMScanActionControllerScanOptimizer*> optimizers, AMAction3 *scanActionTree);
	bool validate(QList<AMScanActionControllerScanValidator*> validators, AMAction3 *scanActionTree);

	extern QList<AMActionRunnerGroup*> actionRunnerGroups_;
	void addActionRunnerGroup(const QString &databaseName, AMActionRunner3 *actionRunner, AMActionHistoryModel3 *actionHistoryModel);
	AMActionRunner3* actionRunnerFromDatabaseName(const QString &databaseName);
	AMActionHistoryModel3* actionHistoryModelFromDatabaseName(const QString &databaseName);
}

#endif // AMAPPCONTROLLERSUPPORT_H
