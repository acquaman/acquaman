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


#ifndef AMEXPORTEROPTIONGENERALASCIIVIEW_H
#define AMEXPORTEROPTIONGENERALASCIIVIEW_H

#include <QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QCheckBox>
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSpacerItem>
#include <QTabWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QStandardItemModel>

class AMExporterOptionGeneralAscii;

class AMExporterOptionGeneralAsciiView : public QTabWidget
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
	QWidget *tabHeader_;
	QVBoxLayout *verticalLayout_;
	QGroupBox *groupBox_;
	QHBoxLayout *horizontalLayout2_;
	QLabel *headerHelpText_;
	QPushButton *headerHelpButton_;
	QCheckBox *headerIncludeCheck_;
	QPlainTextEdit *headerText_;
	QHBoxLayout *horizontalLayout_;
	QSpacerItem *horizontalSpacer_;
	QToolButton *headerKeywordButton_;
	QWidget *tabDataSources_;
	QVBoxLayout *verticalLayout8_;
	QGroupBox *groupBox2_;
	QHBoxLayout *horizontalLayout3_;
	QLabel *sourcesHelpText_;
	QPushButton *sourcesHelpButton_;
	QCheckBox *includeAllSourcesCheck_;
	QCheckBox *firstColumnOnlyCheck_;
	QCheckBox *separateHigherDimensionSourcesCheck_;
	QCheckBox *higherDimensionsInRows_;
	QFrame *sourcesSelectorFrame_;
	QVBoxLayout *verticalLayout9_;
	QGridLayout *gridLayout_;
	QLabel *detectedSourcesLabel_;
	QLabel *includedSourcesLabel_;
	QVBoxLayout *verticalLayout5_;
	QToolButton *sourcesAddButton_;
	QToolButton *sourcesRemoveButton_;
	QToolButton *sourcesAddExtraButton_;
	QListWidget *sourcesIncludedListWidget_;
	QVBoxLayout *verticalLayout4_;
	QGroupBox *sourcesDetailsBox_;
	QVBoxLayout *verticalLayout3_;
	QCheckBox *sourcesRequiredCheck_;
	QCheckBox *sourcesOmitAxisValuesCheck_;
	QGroupBox *sourcesLocationBox_;
	QVBoxLayout *verticalLayout2_;
	QRadioButton *sourcesMainTableButton_;
	QRadioButton *sourcesSeparateSectionButton_;
	QRadioButton *sourcesSeparateFileButton_;
	QSpacerItem *verticalSpacer3_;
	QListView *sourcesAvailableListView_;
	QWidget *tabLayout_;
	QVBoxLayout *verticalLayout6_;
	QGroupBox *groupBox3_;
	QHBoxLayout *horizontalLayout6_;
	QLabel *layoutHelpText_;
	QPushButton *layoutHelpButton_;
	QFormLayout *formLayout_;
	QCheckBox *columnHeaderCheck_;
	QLineEdit *columnHeaderText_;
	QLabel *columnHeaderHelpLabel_;
	QCheckBox *sectionHeaderCheck_;
	QPlainTextEdit *sectionHeaderText_;
	QLabel *sectionHeaderHelpText_;
	QFrame *line_;
	QLabel *columnSeparatorLabel_;
	QHBoxLayout *horizontalLayout4_;
	QRadioButton *columnSeparatorTabButton_;
	QRadioButton *columnSeparatorSpaceButton_;
	QRadioButton *columnSeparatorCommaButton_;
	QRadioButton *columnSeparatorSemicolonButton_;
	QRadioButton *columnSeparatorOtherButton_;
	QLineEdit *columnSeparatorOtherText_;
	QSpacerItem *horizontalSpacer3_;
	QLabel *endOfLineSeparatorLabel_;
	QHBoxLayout *horizontalLayout5_;
	QRadioButton *newlineCRLFCheck_;
	QRadioButton *newlineLFCheck_;
	QRadioButton *newlineOtherCheck_;
	QLineEdit *newlineOtherText_;
	QSpacerItem *horizontalSpacer2_;
	QWidget *tabFileName_;
	QVBoxLayout *verticalLayout7_;
	QGroupBox *groupBox4_;
	QHBoxLayout *horizontalLayout7_;
	QLabel *fileNamesHelpText_;
	QPushButton *fileNamesHelpButton_;
	QSpacerItem *verticalSpacer_;
	QFormLayout *formLayout2_;
	QLabel *fileNameLabel_;
	QLineEdit *fileNameText_;
	QLabel *fileNameSeparateSectionLabel_;
	QLineEdit *fileNameSeparateSectionText_;
	QSpacerItem *verticalSpacer2_;

protected slots:
	/// disable add button when none selected, enable when are.
	void onSourcesAvailableSelectedChanged();

	/// add source
	void onSourcesAddButtonClicked();
	/// remove source
	void onSourcesRemoveButtonClicked();
	/// dialog prompt and add source
	void onSourcesAddExtraButtonClicked();

	/// if index valid: enable edit boxes, block signals and set info boxes to match source, re-enable signals, enable remove button.  If not valid: disable remove button, disable edit boxes
	void onSourcesSelectedChanged(int index);

	/// if source selected, set its required state
	void onSourcesRequiredClicked(bool isRequired);
	/// if source selected, set its omit state
	void onSourcesOmitAxisValuesClicked(bool isOmitted);
	/// if source selected, set source's organize mode to match locationMode
	void onSourcesLocationChanged(int locationMode);

	/// if separatorButton is not other: set option's separator, disable otherBox. If separatorButton is other, enable otherBox, set option's separator to text of otherBox, selectAll in otherBox.
	void onColumnSeparatorChanged(int separatorButton);
	/// if "other" mode selected: set option's separator to otherText
	void onColumnSeparatorOtherTextChanged(const QString& otherText);

	/// if newlineButton is not "other": set option's newline, disable otherBox.  If newlineButton is "other", enable otherBox, set option's newline to text of otherBox, selectAll in otherBox.
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
private:

};

#endif // AMEXPORTEROPTIONGENERALASCIIVIEW_H
