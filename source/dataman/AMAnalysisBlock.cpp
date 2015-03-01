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


#include "AMAnalysisBlock.h"
#include "util/AMErrorMonitor.h"

AMAnalysisBlock::~AMAnalysisBlock(){}

AMAnalysisBlock::AMAnalysisBlock(const QString& outputName, QObject* parent)
	: AMDbObject(parent), AMDataSource(outputName)
{
	AMDbObject::setName(outputName);
	state_ = AMDataSource::InvalidFlag;
	scan_ = 0;
}

void AMAnalysisBlock::setName(const QString &name)
{
	AMDbObject::setName(name);
	AMDataSource::name_ = name;
}

bool AMAnalysisBlock::setInputDataSources(const QList<AMDataSource*>& dataSources) {
	// if a non-empty set of data sources has been provided, and they are not acceptable, return false.  (An empty list must always be acceptable)
	if(!dataSources.isEmpty() && !areInputDataSourcesAcceptable(dataSources)) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -98, QString("There was an error connecting the input data sources to this analysis component '%1: %2'. The data sources provided weren't acceptable. This can happen if they have the wrong dimension, don't provide enough data, etc.").arg(name()).arg(description())));
		return false;
	}

	for(int i=0; i<inputDataSourceCount(); i++) {
		AMDataSource* oldSource = inputDataSourceAt(i);
		disconnect(oldSource->signalSource(), SIGNAL(deleted(void*)), this, SLOT(onInputSourceDeleted(void*)));
		oldSource->deregisterObserver(this);
	}

	for(int i=0; i<dataSources.count(); i++) {
		AMDataSource* newSource = dataSources.at(i);
		connect(newSource->signalSource(), SIGNAL(deleted(void*)), this, SLOT(onInputSourceDeleted(void*)));
		dataSources.at(i)->registerObserver(this);
	}

	setInputDataSourcesImplementation(dataSources);

	emit inputSourcesChanged();
	return true;
}

void AMAnalysisBlock::onInputSourceDeleted(void* deletedSource) {
	// this implementation is just like calling setInputDataSources() with an empty list, except we don't want to call deregisterObserver() on the deleted input source. (In a single-threaded situation, this would be okay, but if the deleted() signal came through a queued signal-slot connection, then that object might already be deleted)
	for(int i=0; i<inputDataSourceCount(); i++) {
		AMDataSource* oldSource = inputDataSourceAt(i);
		if(oldSource != deletedSource) {
			disconnect(oldSource->signalSource(), SIGNAL(deleted(void*)), this, SLOT(onInputSourceDeleted(void*)));
			oldSource->deregisterObserver(this);
		}
	}

	// tell implementation to set its sources to an empty list (inactive/invalid)
	setInputDataSourcesImplementation(QList<AMDataSource*>());
	emit inputSourcesChanged();
}

#include "ui/dataman/AMSimpleDataSourceEditor.h"

QWidget* AMAnalysisBlock::createEditorWidget()
{
	return new AMSimpleDataSourceEditor(this);
}

void AMAnalysisBlock::setVisibleInPlots(bool isVisible){

	if(isVisible == visibleInPlots()) return;

	AMDataSource::setVisibleInPlots(isVisible);
	setModified(true);
}

bool AMAnalysisBlock::loadFromDb(AMDatabase *db, int id)
{
	bool success = AMDbObject::loadFromDb(db, id);

	if(success)
		AMDataSource::name_ = AMDbObject::name();	/// \todo This might change the name of a data-source in mid-life, which is technically not allowed.

	return success;
}

