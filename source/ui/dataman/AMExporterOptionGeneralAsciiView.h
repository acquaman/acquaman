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


#ifndef AMEXPORTEROPTIONGENERALASCIIVIEW_H
#define AMEXPORTEROPTIONGENERALASCIIVIEW_H

#include <QTabWidget>
#include "ui_AMExporterOptionGeneralAsciiView.h"

class QButtonGroup;
class QStandardItemModel;
class AMExporterOptionGeneralAscii;

class AMExporterOptionGeneralAsciiView : public QTabWidget, private Ui::AMExporterOptionGeneralAsciiView
{
	Q_OBJECT

public:
	explicit AMExporterOptionGeneralAsciiView(AMExporterOptionGeneralAscii* option, QStandardItemModel* availableDataSourcesModel = 0, QWidget *parent = 0);
	virtual ~AMExporterOptionGeneralAsciiView();

protected:
	AMExporterOptionGeneralAscii* option_;

	void initializeViewFromOption();
	void makeUIConnections();
	void makeOptionConnections();


	QButtonGroup* dataSourceLocationGroup_, *columnSeparatorGroup_, *newlineGroup_;


protected slots:
	/// disable add button when none selected; enable when are.
	void onSourcesAvailableSelectedChanged();

	/// add source
	void onSourcesAddButtonClicked();
	/// remove source
	void onSourcesRemoveButtonClicked();
	/// dialog prompt and add source
	void onSourcesAddExtraButtonClicked();

	/// if index valid: enable edit boxes; block signals and set info boxes to match source; re-enable signals; enable remove button.  If not valid: disable remove button, disable edit boxes
	void onSourcesSelectedChanged(int index);

	/// if source selected, set its required state
	void onSourcesRequiredClicked(bool isRequired);
	/// if source selected, set its omit state
	void onSourcesOmitAxisValuesClicked(bool isOmitted);
	/// if source selected, set source's organize mode to match locationMode
	void onSourcesLocationChanged(int locationMode);

	/// if separatorButton is not other: set option's separator; disable otherBox. If separatorButton is other, enable otherBox, set option's separator to text of otherBox, selectAll in otherBox.
	void onColumnSeparatorChanged(int separatorButton);
	/// if "other" mode selected: set option's separator to otherText
	void onColumnSeparatorOtherTextChanged(const QString& otherText);

	/// if newlineButton is not "other": set option's newline; disable otherBox.  If newlineButton is "other", enable otherBox, set option's newline to text of otherBox; selectAll in otherBox.
	void onNewlineButtonChanged(int newlineButton);
	/// if "other" mode selected: set option's newline to otherText.
	void onNewlineOtherTextChanged(const QString& otherText);


	/// Grab the most recent text from header block and store in option
	void onHeaderTextChanged();
	/// Grab the most recent text from the section header and store in option
	void onSectionHeaderTextChanged();

	/// Handles setting the option for higher dimensions being written in columns.
	void onHigherDimensionsInRowsToggled(bool checked);

private slots:
};

#endif // AMEXPORTEROPTIONGENERALASCIIVIEW_H
