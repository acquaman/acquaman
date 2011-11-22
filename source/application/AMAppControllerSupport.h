#ifndef AMAPPCONTROLLERSUPPORT_H
#define AMAPPCONTROLLERSUPPORT_H

#include <QString>
#include <QHash>

class QMetaObject;

class AMScanConfiguration;
class AMExporter;
class AMExporterOption;

class AMScanConfigurationObjectInfo{
public:
	/// The default constructor creates an invalid object
	AMScanConfigurationObjectInfo(){
		scanConfigurationMetaObject = 0;
		exporterMetaObject = 0;
		exporterOptionMetaObject = 0;
	}

	/// fill the className
	AMScanConfigurationObjectInfo(AMScanConfiguration *prototypeScanConfiguration, AMExporter *prototypeExporter, AMExporterOption *prototypeExporterOption);

	/// fill the className (This version doesn't require an instance. The \c classMetaObject can be retrieved statically with Class::staticMetaObject. )
	AMScanConfigurationObjectInfo(const QMetaObject *scanConfigurationMetaObject, const QMetaObject *exporterMetaObject, const QMetaObject *exporterOptionMetaObject);

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

private:
	/// used to implement both constructors
	void initWithMetaObject(const QMetaObject *scanConfigurationMetaObject, const QMetaObject *exporterMetaObject, const QMetaObject *exporterOptionMetaObject);

	/// checks to make sure a QMetaObject inherits AMScanConfiguration
	bool inheritsScanConfiguration(const QMetaObject *metaObject) const;

	/// checks to make sure a QMetaObject inherits AMExporter
	bool inheritsExporter(const QMetaObject *metaObject) const;

	/// checks to make sure a QMetaObject inherits AMExporterOption
	bool inheritsExporterOption(const QMetaObject *metaObject) const;
};

namespace AMAppControllerSupport{
	extern QHash<QString, AMScanConfigurationObjectInfo> registeredClasses_;

	// Registers the triplet of a scan configuration class (AMScanConfiguration descendant), an exporter class (AMExporter descendant), and an exporter option class (AMExporterOption descendent)
	// Class T1 needs to inherit AMScanConfiguration, Class T3 needs to inherit AMExporter, and Class T3 needs to inherit AMExporterOption
	template <class Ta, class Tb, class Tc>
	bool registerClass() {
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
		if(registeredClasses_.contains(className)) {
			//AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, 0, QString("Detector View Support: The class '%1' has already been registered with detector view. Skipping duplicate registration.").arg(className)));
			return true;
		}

		AMScanConfigurationObjectInfo newInfo(scanConfigurationMo, exporterMo, exporterOptionMo);

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
}

#endif // AMAPPCONTROLLERSUPPORT_H
