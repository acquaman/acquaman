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


#include "AMAppControllerSupport.h"

#include <QMetaObject>

#include "acquaman/AMScanConfiguration.h"
#include "dataman/export/AMExporter.h"
#include "dataman/export/AMExporterOption.h"

#include "acquaman/AMScanActionControllerScanOptimizer.h"
#include "acquaman/AMScanActionControllerScanValidator.h"

 AMScanConfigurationObjectInfo::~AMScanConfigurationObjectInfo(){}
AMScanConfigurationObjectInfo::AMScanConfigurationObjectInfo(AMScanConfiguration *prototypeScanConfiguration, AMExporter *prototypeExporter, AMExporterOption *prototypeExporterOption, int useExporterOptionId, QString useDatabaseName){
	initWithMetaObject(prototypeScanConfiguration->metaObject(), prototypeExporter->metaObject(), prototypeExporterOption->metaObject(), useExporterOptionId, useDatabaseName);
}

AMScanConfigurationObjectInfo::AMScanConfigurationObjectInfo(const QMetaObject *useScanConfigurationMetaObject, const QMetaObject *useExporterMetaObject, const QMetaObject *useExporterOptionMetaObject, int useExporterOptionId, QString useDatabaseName){
	if(inheritsScanConfiguration(useScanConfigurationMetaObject) && inheritsExporter(useExporterMetaObject) && inheritsExporterOption(useExporterOptionMetaObject) && (useExporterOptionId >= 1))
		initWithMetaObject(useScanConfigurationMetaObject, useExporterMetaObject, useExporterOptionMetaObject, useExporterOptionId, useDatabaseName);
	else{
		scanConfigurationMetaObject = 0;
		exporterMetaObject = 0;
		exporterOptionMetaObject = 0;
		exporterOptionId = 0;
		databaseName = QString();
	}
}

void AMScanConfigurationObjectInfo::initWithMetaObject(const QMetaObject *useScanConfigurationMetaObject, const QMetaObject *useExporterMetaObject, const QMetaObject *useExporterOptionMetaObject, int useExporterOptionId, QString useDatabaseName){
	scanConfigurationMetaObject = useScanConfigurationMetaObject;
	exporterMetaObject = useExporterMetaObject;
	exporterOptionMetaObject = useExporterOptionMetaObject;
	exporterOptionId = useExporterOptionId;
	if (useDatabaseName.isEmpty())
		databaseName = "user";
	else
		databaseName = useDatabaseName;
	scanConfigurationClassName = useScanConfigurationMetaObject->className();
	exporterClassName = useExporterMetaObject->className();
	exporterOptionClassName = useExporterOptionMetaObject->className();
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
		QHash<QString, AMScanConfigurationObjectInfo>::const_iterator i = registeredClasses_.constBegin();
		while( i != registeredClasses_.constEnd()){
			if(i.value().scanConfigurationClassName == scanConfiguration->getMetaObject()->className()){
				AMExporter *exporter = qobject_cast<AMExporter*>(i.value().exporterMetaObject->newInstance());
				if(exporter)
					return exporter;
			}

			i++;
		}
		return 0;
	}

	AMExporterOption* createExporterOption(AMScanConfiguration *scanConfiguration){
		if(!scanConfiguration)
			return 0;
		QHash<QString, AMScanConfigurationObjectInfo>::const_iterator i = registeredClasses_.constBegin();
		while( i != registeredClasses_.constEnd()){
			if(i.value().scanConfigurationClassName == scanConfiguration->getMetaObject()->className()){
				AMExporterOption *exporterOption = qobject_cast<AMExporterOption*>(i.value().exporterOptionMetaObject->newInstance());
				QString useAsDatabase = QString();
				if(!i.value().databaseName.isEmpty())
					useAsDatabase = i.value().databaseName;
				if(exporterOption && exporterOption->loadFromDb(AMDatabase::database(useAsDatabase), i.value().exporterOptionId))
					return exporterOption;
			}

			i++;
		}
		return 0;
	}



	QList<AMScanActionControllerScanOptimizer*> principleOptimizers_;
	QList<AMScanActionControllerScanValidator*> principleValidators_;

	int principleOptimizerCount(){
		return principleOptimizers_.count();
	}

	int principleValidatorCount(){
		return principleValidators_.count();
	}

	AMScanActionControllerScanOptimizer* principleOptimizerAt(int index){
		if(index < 0 || index >= principleOptimizers_.count())
			return 0;
		return principleOptimizers_.at(index);
	}

	AMScanActionControllerScanValidator* principleValidatorAt(int index){
		if(index < 0 || index >= principleValidators_.count())
			return 0;
		return principleValidators_.at(index);
	}

	void addPrincipleOptimizer(int index, AMScanActionControllerScanOptimizer *optimizer){
		if(index < -1 || index > principleOptimizers_.count() || !optimizer)
			return;
		principleOptimizers_.insert(index, optimizer);
	}

	void addPrincipleValidator(int index, AMScanActionControllerScanValidator *validator){
		if(index < -1 || index > principleValidators_.count() || !validator)
			return;
		principleValidators_.insert(index, validator);
	}

	void appendPrincipleOptimizer(AMScanActionControllerScanOptimizer *optimizer){
		if(!optimizer)
			return;
		principleOptimizers_.append(optimizer);
	}

	void appendPrincipleValidator(AMScanActionControllerScanValidator *validator){
		if(!validator)
			return;
		principleValidators_.append(validator);
	}

	void prependPrincipleOptimizer(AMScanActionControllerScanOptimizer *optimizer){
		if(!optimizer)
			return;
		principleOptimizers_.prepend(optimizer);
	}

	void prependPrincipleValidator(AMScanActionControllerScanValidator *validator){
		if(!validator)
			return;
		principleValidators_.prepend(validator);
	}

	AMScanActionControllerScanOptimizer* removePrincipleOptimizer(int index){
		if(index < 0 || index >= principleOptimizers_.count())
			return 0;
		return principleOptimizers_.takeAt(index);
	}

	AMScanActionControllerScanValidator* removePrincipleValidator(int index){
		if(index < 0 || index >= principleValidators_.count())
			return 0;
		return principleValidators_.takeAt(index);
	}

	QList<AMScanActionControllerScanOptimizer*> principleOptimizersCopy(){
		QList<AMScanActionControllerScanOptimizer*> principleOptimizersCopy;
		for(int x = 0; x < principleOptimizers_.count(); x++)
			principleOptimizersCopy.append(principleOptimizers_.at(x));
		return principleOptimizersCopy;
	}

	QList<AMScanActionControllerScanValidator*> principleValidatorsCopy(){
		QList<AMScanActionControllerScanValidator*> principleValidatorsCopy;
		for(int x = 0; x < principleValidators_.count(); x++)
			principleValidatorsCopy.append(principleValidators_.at(x));
		return principleValidatorsCopy;
	}

	void optimize(QList<AMScanActionControllerScanOptimizer *> optimizers, AMAction3 *scanActionTree){
		if(!scanActionTree)
			return;
		for(int x = 0; x < optimizers.count(); x++){
			optimizers.at(x)->setScanActionTree(scanActionTree);
			optimizers.at(x)->optimize();
		}
	}

	bool validate(QList<AMScanActionControllerScanValidator *> validators, AMAction3 *scanActionTree){
		if(!scanActionTree)
			return false;
		for(int x = 0; x < validators.count(); x++){
			validators.at(x)->setScanActionTree(scanActionTree);
			if(!validators.at(x)->validate())
				return false;
		}
		return true;
	}

	QList<AMActionRunnerGroup*> actionRunnerGroups_;

	void addActionRunnerGroup(const QString &databaseName, AMActionRunner3 *actionRunner, AMActionHistoryModel3 *actionHistoryModel){
		actionRunnerGroups_.append(new AMActionRunnerGroup(databaseName, actionRunner, actionHistoryModel));
	}

	AMActionRunner3* actionRunnerFromDatabaseName(const QString &databaseName){
		for(int x = 0; x < actionRunnerGroups_.count(); x++)
			if(actionRunnerGroups_.at(x)->databaseName() == databaseName)
				return actionRunnerGroups_.at(x)->actionRunner();

		return 0;
	}

	AMActionHistoryModel3* actionHistoryModelFromDatabaseName(const QString &databaseName){
		for(int x = 0; x < actionRunnerGroups_.count(); x++)
			if(actionRunnerGroups_.at(x)->databaseName() == databaseName)
				return actionRunnerGroups_.at(x)->actionHistoryModel();

		return 0;
	}
}

 AMActionRunnerGroup::~AMActionRunnerGroup(){}
