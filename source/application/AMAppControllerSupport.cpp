#include "AMAppControllerSupport.h"

#include <QMetaObject>

#include "acquaman/AMScanConfiguration.h"
#include "dataman/export/AMExporter.h"
#include "dataman/export/AMExporterOption.h"

AMScanConfigurationObjectInfo::AMScanConfigurationObjectInfo(AMScanConfiguration *prototypeScanConfiguration, AMExporter *prototypeExporter, AMExporterOption *prototypeExporterOption){
	initWithMetaObject(prototypeScanConfiguration->metaObject(), prototypeExporter->metaObject(), prototypeExporterOption->metaObject());
}

AMScanConfigurationObjectInfo::AMScanConfigurationObjectInfo(const QMetaObject *scanConfigurationMetaObject, const QMetaObject *exporterMetaObject, const QMetaObject *exporterOptionMetaObject){
	if(inheritsScanConfiguration(scanConfigurationMetaObject) && inheritsExporter(exporterMetaObject) && inheritsExporterOption(exporterOptionMetaObject))
		initWithMetaObject(scanConfigurationMetaObject, exporterMetaObject, exporterOptionMetaObject);
	else{
		scanConfigurationMetaObject = 0;
		exporterMetaObject = 0;
		exporterOptionMetaObject = 0;
	}
}

void AMScanConfigurationObjectInfo::initWithMetaObject(const QMetaObject *scanConfigurationMetaObject, const QMetaObject *exporterMetaObject, const QMetaObject *exporterOptionMetaObject){
	scanConfigurationMetaObject = scanConfigurationMetaObject;
	exporterMetaObject = exporterMetaObject;
	exporterOptionMetaObject = exporterOptionMetaObject;
	scanConfigurationClassName = scanConfigurationMetaObject->className();
	exporterClassName = exporterMetaObject->className();
	exporterClassName = exporterOptionMetaObject->className();
}

bool AMScanConfigurationObjectInfo::inheritsScanConfiguration(const QMetaObject *metaObject) const{
	// is this a subclass of AMScanConfiguration? (Or an AMScanConfiguration itself?)
	const QMetaObject* superClass = metaObject;
	bool inheritsScanConfiguration;
	do {
		inheritsScanConfiguration = (superClass->className() == QString("AMScanConfiguration"));
	}
	while( (superClass=superClass->superClass()) && inheritsScanConfiguration == false );
	return inheritsScanConfiguration;
}

bool AMScanConfigurationObjectInfo::inheritsExporter(const QMetaObject *metaObject) const{
	// is this a subclass of AMExporter? (Or an AMExporter itself?)
	const QMetaObject* superClass = metaObject;
	bool inheritsExporter;
	do {
		inheritsExporter = (superClass->className() == QString("AMExporter"));
	}
	while( (superClass=superClass->superClass()) && inheritsExporter == false );
	return inheritsExporter;
}

bool AMScanConfigurationObjectInfo::inheritsExporterOption(const QMetaObject *metaObject) const{
	// is this a subclass of AMExporterOption? (Or an AMExporterOption itself?)
	const QMetaObject* superClass = metaObject;
	bool inheritsExporterOption;
	do {
		inheritsExporterOption = (superClass->className() == QString("AMExporterOption"));
	}
	while( (superClass=superClass->superClass()) && inheritsExporterOption == false );
	return inheritsExporterOption;
}

namespace AMAppControllerSupport{
	QHash<QString, AMScanConfigurationObjectInfo> registeredClasses_;

	const QHash<QString, AMScanConfigurationObjectInfo>* registeredClasses() {
		return &registeredClasses_;
	}

	AMExporter* createExporter(AMScanConfiguration *scanConfiguration){
		if(!scanConfiguration)
			return 0;
		//qDebug() << "Trying to create view for detector named " << detector->detectorName();
		QHash<QString, AMScanConfigurationObjectInfo>::const_iterator i = registeredClasses_.constBegin();
		while( i != registeredClasses_.constEnd()){
			if(i.value().scanConfigurationClassName == scanConfiguration->getMetaObject()->className()){
				AMExporter *exporter = qobject_cast<AMExporter*>(i.value().exporterMetaObject->newInstance());
				if(exporter)
					return exporter;
			}
		}
		return 0;
	}

	AMExporterOption* createExporterOption(AMScanConfiguration *scanConfiguration){
		if(!scanConfiguration)
			return 0;
		//qDebug() << "Trying to create view for detector named " << detector->detectorName();
		QHash<QString, AMScanConfigurationObjectInfo>::const_iterator i = registeredClasses_.constBegin();
		while( i != registeredClasses_.constEnd()){
			if(i.value().scanConfigurationClassName == scanConfiguration->getMetaObject()->className()){
				AMExporterOption *exporterOption = qobject_cast<AMExporterOption*>(i.value().exporterOptionMetaObject->newInstance());
				if(exporterOption)
					return exporterOption;
			}
		}
		return 0;
	}
}

