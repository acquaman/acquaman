/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMSIMPLEDATASOURCEEDITOR_H
#define AMSIMPLEDATASOURCEEDITOR_H

#include <QWidget>
#include "dataman/datasource/AMDataSource.h"

class QLabel;

/// Provides an extremely simple widget as the default for AMDataSource::createEditorWidget(), in analysis blocks and raw data sources. For 0D data sources, displays the value.  For nD data sources, displays the rank and dimensions.
class AMSimpleDataSourceEditor : public QWidget
{
    Q_OBJECT
public:
	/// Constructor requires a valid data source \c dataSource.
 	virtual ~AMSimpleDataSourceEditor();
	AMSimpleDataSourceEditor(AMDataSource* dataSource, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Called when the value of a 0D data source changes.
	void onValuesChanged();
	/// Called when the size of an nD data source changes.
	void onSizeChanged();

protected:
	QLabel* valueLabel_, *value_;
	QLabel* rankLabel_, *rank_;
	QLabel* sizeLabel_, *size_;

	AMDataSource* dataSource_;

};

#endif // AMSIMPLEDATASOURCEEDITOR_H
