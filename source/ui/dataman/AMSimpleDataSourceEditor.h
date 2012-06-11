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
