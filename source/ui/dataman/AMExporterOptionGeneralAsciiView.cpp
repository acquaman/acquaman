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


#include "AMExporterOptionGeneralAsciiView.h"
#include "ui_AMExporterOptionGeneralAsciiView.h"

#include "dataman/export/AMExporterOptionGeneralAscii.h"

#include <QButtonGroup>
#include <QInputDialog>

AMExporterOptionGeneralAsciiView::AMExporterOptionGeneralAsciiView(AMExporterOptionGeneralAscii* option, QStandardItemModel* availableDataSourcesModel, QWidget *parent) :
	QTabWidget(parent)
{
	if (objectName().isEmpty())
		setObjectName(QString::fromUtf8("AMExporterOptionGeneralAsciiView"));
	resize(632, 420);
	setMinimumSize(QSize(0, 420));
	tabHeader_ = new QWidget();
	tabHeader_->setObjectName(QString::fromUtf8("tabHeader_"));
	verticalLayout = new QVBoxLayout(tabHeader_);
	verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
	groupBox_ = new QGroupBox(tabHeader_);
	groupBox_->setObjectName(QString::fromUtf8("groupBox_"));
	horizontalLayout_2 = new QHBoxLayout(groupBox_);
	horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
	headerHelpText_ = new QLabel(groupBox_);
	headerHelpText_->setObjectName(QString::fromUtf8("headerHelpText_"));
	headerHelpText_->setWordWrap(true);

	horizontalLayout_2->addWidget(headerHelpText_);

	headerHelpButton_ = new QPushButton(groupBox_);
	headerHelpButton_->setObjectName(QString::fromUtf8("headerHelpButton_"));
	QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(headerHelpButton_->sizePolicy().hasHeightForWidth());
	headerHelpButton_->setSizePolicy(sizePolicy);

	horizontalLayout_2->addWidget(headerHelpButton_);


	verticalLayout->addWidget(groupBox_);

	headerIncludeCheck_ = new QCheckBox(tabHeader_);
	headerIncludeCheck_->setObjectName(QString::fromUtf8("headerIncludeCheck_"));
	headerIncludeCheck_->setChecked(true);

	verticalLayout->addWidget(headerIncludeCheck_);

	headerText_ = new QPlainTextEdit(tabHeader_);
	headerText_->setObjectName(QString::fromUtf8("headerText_"));

	verticalLayout->addWidget(headerText_);

	horizontalLayout_ = new QHBoxLayout();
	horizontalLayout_->setObjectName(QString::fromUtf8("horizontalLayout_"));
	horizontalSpacer_ = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	horizontalLayout_->addItem(horizontalSpacer_);

	headerKeywordButton_ = new QToolButton(tabHeader_);
	headerKeywordButton_->setObjectName(QString::fromUtf8("headerKeywordButton_"));

	horizontalLayout_->addWidget(headerKeywordButton_);


	verticalLayout->addLayout(horizontalLayout_);

	addTab(tabHeader_, QString());
	tabDataSources_ = new QWidget();
	tabDataSources_->setObjectName(QString::fromUtf8("tabDataSources_"));
	verticalLayout_8 = new QVBoxLayout(tabDataSources_);
	verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
	groupBox2_ = new QGroupBox(tabDataSources_);
	groupBox2_->setObjectName(QString::fromUtf8("groupBox2_"));
	horizontalLayout_3 = new QHBoxLayout(groupBox2_);
	horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
	sourcesHelpText_ = new QLabel(groupBox2_);
	sourcesHelpText_->setObjectName(QString::fromUtf8("sourcesHelpText_"));

	horizontalLayout_3->addWidget(sourcesHelpText_);

	sourcesHelpButton_ = new QPushButton(groupBox2_);
	sourcesHelpButton_->setObjectName(QString::fromUtf8("sourcesHelpButton_"));
	sizePolicy.setHeightForWidth(sourcesHelpButton_->sizePolicy().hasHeightForWidth());
	sourcesHelpButton_->setSizePolicy(sizePolicy);

	horizontalLayout_3->addWidget(sourcesHelpButton_);


	verticalLayout_8->addWidget(groupBox2_);

	includeAllSourcesCheck_ = new QCheckBox(tabDataSources_);
	includeAllSourcesCheck_->setObjectName(QString::fromUtf8("includeAllSourcesCheck_"));

	verticalLayout_8->addWidget(includeAllSourcesCheck_);

	firstColumnOnlyCheck_ = new QCheckBox(tabDataSources_);
	firstColumnOnlyCheck_->setObjectName(QString::fromUtf8("firstColumnOnlyCheck_"));
	firstColumnOnlyCheck_->setEnabled(true);

	verticalLayout_8->addWidget(firstColumnOnlyCheck_);

	separateHigherDimensionSourcesCheck_ = new QCheckBox(tabDataSources_);
	separateHigherDimensionSourcesCheck_->setObjectName(QString::fromUtf8("separateHigherDimensionSourcesCheck_"));

	verticalLayout_8->addWidget(separateHigherDimensionSourcesCheck_);

	higherDimensionsInRows_ = new QCheckBox(tabDataSources_);
	higherDimensionsInRows_->setObjectName(QString::fromUtf8("higherDimensionsInRows_"));
	higherDimensionsInRows_->setChecked(false);

	verticalLayout_8->addWidget(higherDimensionsInRows_);

	sourcesSelectorFrame_ = new QFrame(tabDataSources_);
	sourcesSelectorFrame_->setObjectName(QString::fromUtf8("sourcesSelectorFrame_"));
	sourcesSelectorFrame_->setEnabled(false);
	sourcesSelectorFrame_->setFrameShape(QFrame::StyledPanel);
	sourcesSelectorFrame_->setFrameShadow(QFrame::Raised);
	verticalLayout_9 = new QVBoxLayout(sourcesSelectorFrame_);
	verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
	gridLayout_ = new QGridLayout();
	gridLayout_->setObjectName(QString::fromUtf8("gridLayout_"));
	detectedSourcesLabel_ = new QLabel(sourcesSelectorFrame_);
	detectedSourcesLabel_->setObjectName(QString::fromUtf8("detectedSourcesLabel_"));

	gridLayout_->addWidget(detectedSourcesLabel_, 0, 0, 1, 1);

	includedSourcesLabel_ = new QLabel(sourcesSelectorFrame_);
	includedSourcesLabel_->setObjectName(QString::fromUtf8("includedSourcesLabel_"));

	gridLayout_->addWidget(includedSourcesLabel_, 0, 2, 1, 1);

	verticalLayout5_ = new QVBoxLayout();
	verticalLayout5_->setObjectName(QString::fromUtf8("verticalLayout5_"));
	sourcesAddButton_ = new QToolButton(sourcesSelectorFrame_);
	sourcesAddButton_->setObjectName(QString::fromUtf8("sourcesAddButton_"));
	sourcesAddButton_->setEnabled(false);
	QIcon icon;
	icon.addFile(QString::fromUtf8(":/add.png"), QSize(), QIcon::Normal, QIcon::Off);
	sourcesAddButton_->setIcon(icon);

	verticalLayout5_->addWidget(sourcesAddButton_);

	sourcesRemoveButton_ = new QToolButton(sourcesSelectorFrame_);
	sourcesRemoveButton_->setObjectName(QString::fromUtf8("sourcesRemoveButton_"));
	sourcesRemoveButton_->setEnabled(false);
	QIcon icon1;
	icon1.addFile(QString::fromUtf8(":/Close.png"), QSize(), QIcon::Normal, QIcon::Off);
	sourcesRemoveButton_->setIcon(icon1);

	verticalLayout5_->addWidget(sourcesRemoveButton_);


	gridLayout_->addLayout(verticalLayout5_, 1, 1, 1, 1);

	sourcesAddExtraButton_ = new QToolButton(sourcesSelectorFrame_);
	sourcesAddExtraButton_->setObjectName(QString::fromUtf8("sourcesAddExtraButton_"));

	gridLayout_->addWidget(sourcesAddExtraButton_, 2, 3, 1, 1);

	sourcesIncludedListWidget_ = new QListWidget(sourcesSelectorFrame_);
	sourcesIncludedListWidget_->setObjectName(QString::fromUtf8("sourcesIncludedListWidget_"));

	gridLayout_->addWidget(sourcesIncludedListWidget_, 1, 2, 1, 2);

	verticalLayout4_ = new QVBoxLayout();
	verticalLayout4_->setObjectName(QString::fromUtf8("verticalLayout4_"));
	sourcesDetailsBox_ = new QGroupBox(sourcesSelectorFrame_);
	sourcesDetailsBox_->setObjectName(QString::fromUtf8("sourcesDetailsBox_"));
	sourcesDetailsBox_->setEnabled(false);
	verticalLayout_3 = new QVBoxLayout(sourcesDetailsBox_);
	verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
	verticalLayout_3->setContentsMargins(-1, 6, -1, 6);
	sourcesRequiredCheck_ = new QCheckBox(sourcesDetailsBox_);
	sourcesRequiredCheck_->setObjectName(QString::fromUtf8("sourcesRequiredCheck_"));

	verticalLayout_3->addWidget(sourcesRequiredCheck_);

	sourcesOmitAxisValuesCheck_ = new QCheckBox(sourcesDetailsBox_);
	sourcesOmitAxisValuesCheck_->setObjectName(QString::fromUtf8("sourcesOmitAxisValuesCheck_"));
	sourcesOmitAxisValuesCheck_->setChecked(false);

	verticalLayout_3->addWidget(sourcesOmitAxisValuesCheck_);


	verticalLayout4_->addWidget(sourcesDetailsBox_);

	sourcesLocationBox_ = new QGroupBox(sourcesSelectorFrame_);
	sourcesLocationBox_->setObjectName(QString::fromUtf8("sourcesLocationBox_"));
	sourcesLocationBox_->setEnabled(false);
	verticalLayout_2 = new QVBoxLayout(sourcesLocationBox_);
	verticalLayout_2->setSpacing(8);
	verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
	verticalLayout_2->setContentsMargins(-1, 6, -1, 6);
	sourcesMainTableButton_ = new QRadioButton(sourcesLocationBox_);
	sourcesMainTableButton_->setObjectName(QString::fromUtf8("sourcesMainTableButton_"));
	sourcesMainTableButton_->setMinimumSize(QSize(0, 20));

	verticalLayout_2->addWidget(sourcesMainTableButton_);

	sourcesSeparateSectionButton_ = new QRadioButton(sourcesLocationBox_);
	sourcesSeparateSectionButton_->setObjectName(QString::fromUtf8("sourcesSeparateSectionButton_"));
	sourcesSeparateSectionButton_->setMinimumSize(QSize(0, 20));

	verticalLayout_2->addWidget(sourcesSeparateSectionButton_);

	sourcesSeparateFileButton_ = new QRadioButton(sourcesLocationBox_);
	sourcesSeparateFileButton_->setObjectName(QString::fromUtf8("sourcesSeparateFileButton_"));
	sourcesSeparateFileButton_->setMinimumSize(QSize(0, 20));

	verticalLayout_2->addWidget(sourcesSeparateFileButton_);


	verticalLayout4_->addWidget(sourcesLocationBox_);

	verticalSpacer3_ = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

	verticalLayout4_->addItem(verticalSpacer3_);


	gridLayout_->addLayout(verticalLayout4_, 1, 4, 2, 1);

	sourcesAvailableListView_ = new QListView(sourcesSelectorFrame_);
	sourcesAvailableListView_->setObjectName(QString::fromUtf8("sourcesAvailableListView_"));
	sourcesAvailableListView_->setSelectionMode(QAbstractItemView::ExtendedSelection);

	gridLayout_->addWidget(sourcesAvailableListView_, 1, 0, 1, 1);


	verticalLayout_9->addLayout(gridLayout_);


	verticalLayout_8->addWidget(sourcesSelectorFrame_);

	addTab(tabDataSources_, QString());
	tabLayout_ = new QWidget();
	tabLayout_->setObjectName(QString::fromUtf8("tabLayout_"));
	verticalLayout_6 = new QVBoxLayout(tabLayout_);
	verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
	groupBox3_ = new QGroupBox(tabLayout_);
	groupBox3_->setObjectName(QString::fromUtf8("groupBox3_"));
	horizontalLayout_6 = new QHBoxLayout(groupBox3_);
	horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
	layoutHelpText_ = new QLabel(groupBox3_);
	layoutHelpText_->setObjectName(QString::fromUtf8("layoutHelpText_"));
	layoutHelpText_->setWordWrap(true);

	horizontalLayout_6->addWidget(layoutHelpText_);

	layoutHelpButton_ = new QPushButton(groupBox3_);
	layoutHelpButton_->setObjectName(QString::fromUtf8("layoutHelpButton_"));
	sizePolicy.setHeightForWidth(layoutHelpButton_->sizePolicy().hasHeightForWidth());
	layoutHelpButton_->setSizePolicy(sizePolicy);

	horizontalLayout_6->addWidget(layoutHelpButton_);


	verticalLayout_6->addWidget(groupBox3_);

	formLayout_ = new QFormLayout();
	formLayout_->setObjectName(QString::fromUtf8("formLayout_"));
	formLayout_->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
	columnHeaderCheck_ = new QCheckBox(tabLayout_);
	columnHeaderCheck_->setObjectName(QString::fromUtf8("columnHeaderCheck_"));
	columnHeaderCheck_->setChecked(true);

	formLayout_->setWidget(0, QFormLayout::LabelRole, columnHeaderCheck_);

	columnHeaderText_ = new QLineEdit(tabLayout_);
	columnHeaderText_->setObjectName(QString::fromUtf8("columnHeaderText_"));

	formLayout_->setWidget(0, QFormLayout::FieldRole, columnHeaderText_);

	columnHeaderHelpLabel_ = new QLabel(tabLayout_);
	columnHeaderHelpLabel_->setObjectName(QString::fromUtf8("columnHeaderHelpLabel_"));
	QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
	sizePolicy1.setHorizontalStretch(0);
	sizePolicy1.setVerticalStretch(0);
	sizePolicy1.setHeightForWidth(columnHeaderHelpLabel_->sizePolicy().hasHeightForWidth());
	columnHeaderHelpLabel_->setSizePolicy(sizePolicy1);
	columnHeaderHelpLabel_->setWordWrap(true);

	formLayout_->setWidget(1, QFormLayout::FieldRole, columnHeaderHelpLabel_);

	sectionHeaderCheck_ = new QCheckBox(tabLayout_);
	sectionHeaderCheck_->setObjectName(QString::fromUtf8("sectionHeaderCheck_"));
	sectionHeaderCheck_->setChecked(true);

	formLayout_->setWidget(2, QFormLayout::LabelRole, sectionHeaderCheck_);

	sectionHeaderText_ = new QPlainTextEdit(tabLayout_);
	sectionHeaderText_->setObjectName(QString::fromUtf8("sectionHeaderText_"));
	QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
	sizePolicy2.setHorizontalStretch(0);
	sizePolicy2.setVerticalStretch(0);
	sizePolicy2.setHeightForWidth(sectionHeaderText_->sizePolicy().hasHeightForWidth());
	sectionHeaderText_->setSizePolicy(sizePolicy2);
	sectionHeaderText_->setMaximumSize(QSize(16777215, 80));

	formLayout_->setWidget(2, QFormLayout::FieldRole, sectionHeaderText_);

	sectionHeaderHelpText_ = new QLabel(tabLayout_);
	sectionHeaderHelpText_->setObjectName(QString::fromUtf8("sectionHeaderHelpText_"));
	sizePolicy1.setHeightForWidth(sectionHeaderHelpText_->sizePolicy().hasHeightForWidth());
	sectionHeaderHelpText_->setSizePolicy(sizePolicy1);
	sectionHeaderHelpText_->setWordWrap(true);

	formLayout_->setWidget(3, QFormLayout::FieldRole, sectionHeaderHelpText_);

	line_ = new QFrame(tabLayout_);
	line_->setObjectName(QString::fromUtf8("line_"));
	line_->setFrameShape(QFrame::HLine);
	line_->setFrameShadow(QFrame::Sunken);

	formLayout_->setWidget(4, QFormLayout::SpanningRole, line_);

	columnSeparatorLabel_ = new QLabel(tabLayout_);
	columnSeparatorLabel_->setObjectName(QString::fromUtf8("columnSeparatorLabel_"));

	formLayout_->setWidget(5, QFormLayout::LabelRole, columnSeparatorLabel_);

	horizontalLayout4_ = new QHBoxLayout();
	horizontalLayout4_->setSpacing(6);
	horizontalLayout4_->setObjectName(QString::fromUtf8("horizontalLayout4_"));
	columnSeparatorTabButton_ = new QRadioButton(tabLayout_);
	columnSeparatorTabButton_->setObjectName(QString::fromUtf8("columnSeparatorTabButton_"));
	columnSeparatorTabButton_->setChecked(true);

	horizontalLayout4_->addWidget(columnSeparatorTabButton_);

	columnSeparatorSpaceButton_ = new QRadioButton(tabLayout_);
	columnSeparatorSpaceButton_->setObjectName(QString::fromUtf8("columnSeparatorSpaceButton_"));

	horizontalLayout4_->addWidget(columnSeparatorSpaceButton_);

	columnSeparatorCommaButton_ = new QRadioButton(tabLayout_);
	columnSeparatorCommaButton_->setObjectName(QString::fromUtf8("columnSeparatorCommaButton_"));

	horizontalLayout4_->addWidget(columnSeparatorCommaButton_);

	columnSeparatorSemicolonButton_ = new QRadioButton(tabLayout_);
	columnSeparatorSemicolonButton_->setObjectName(QString::fromUtf8("columnSeparatorSemicolonButton_"));

	horizontalLayout4_->addWidget(columnSeparatorSemicolonButton_);

	columnSeparatorOtherButton_ = new QRadioButton(tabLayout_);
	columnSeparatorOtherButton_->setObjectName(QString::fromUtf8("columnSeparatorOtherButton_"));

	horizontalLayout4_->addWidget(columnSeparatorOtherButton_);

	columnSeparatorOtherText_ = new QLineEdit(tabLayout_);
	columnSeparatorOtherText_->setObjectName(QString::fromUtf8("columnSeparatorOtherText_"));
	columnSeparatorOtherText_->setEnabled(false);
	columnSeparatorOtherText_->setMinimumSize(QSize(40, 0));
	columnSeparatorOtherText_->setMaximumSize(QSize(40, 16777215));

	horizontalLayout4_->addWidget(columnSeparatorOtherText_);

	horizontalSpacer3_ = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

	horizontalLayout4_->addItem(horizontalSpacer3_);


	formLayout_->setLayout(5, QFormLayout::FieldRole, horizontalLayout4_);

	endOfLineSeparatorLabel_ = new QLabel(tabLayout_);
	endOfLineSeparatorLabel_->setObjectName(QString::fromUtf8("endOfLineSeparatorLabel_"));

	formLayout_->setWidget(6, QFormLayout::LabelRole, endOfLineSeparatorLabel_);

	horizontalLayout5_ = new QHBoxLayout();
	horizontalLayout5_->setSpacing(6);
	horizontalLayout5_->setObjectName(QString::fromUtf8("horizontalLayout5_"));
	horizontalLayout5_->setContentsMargins(-1, -1, 0, -1);
	newlineCRLFCheck_ = new QRadioButton(tabLayout_);
	newlineCRLFCheck_->setObjectName(QString::fromUtf8("newlineCRLFCheck_"));
	newlineCRLFCheck_->setChecked(true);

	horizontalLayout5_->addWidget(newlineCRLFCheck_);

	newlineLFCheck_ = new QRadioButton(tabLayout_);
	newlineLFCheck_->setObjectName(QString::fromUtf8("newlineLFCheck_"));
	newlineLFCheck_->setChecked(false);

	horizontalLayout5_->addWidget(newlineLFCheck_);

	newlineOtherCheck_ = new QRadioButton(tabLayout_);
	newlineOtherCheck_->setObjectName(QString::fromUtf8("newlineOtherCheck_"));

	horizontalLayout5_->addWidget(newlineOtherCheck_);

	newlineOtherText_ = new QLineEdit(tabLayout_);
	newlineOtherText_->setObjectName(QString::fromUtf8("newlineOtherText_"));
	newlineOtherText_->setEnabled(false);
	newlineOtherText_->setMinimumSize(QSize(40, 0));
	newlineOtherText_->setMaximumSize(QSize(40, 16777215));

	horizontalLayout5_->addWidget(newlineOtherText_);

	horizontalSpacer2_ = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

	horizontalLayout5_->addItem(horizontalSpacer2_);


	formLayout_->setLayout(6, QFormLayout::FieldRole, horizontalLayout5_);


	verticalLayout_6->addLayout(formLayout_);

	addTab(tabLayout_, QString());
	tabFileName_ = new QWidget();
	tabFileName_->setObjectName(QString::fromUtf8("tabFileName_"));
	verticalLayout_7 = new QVBoxLayout(tabFileName_);
	verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
	groupBox4_ = new QGroupBox(tabFileName_);
	groupBox4_->setObjectName(QString::fromUtf8("groupBox4_"));
	horizontalLayout_7 = new QHBoxLayout(groupBox4_);
	horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
	fileNamesHelpText_ = new QLabel(groupBox4_);
	fileNamesHelpText_->setObjectName(QString::fromUtf8("fileNamesHelpText_"));
	fileNamesHelpText_->setWordWrap(true);

	horizontalLayout_7->addWidget(fileNamesHelpText_);

	fileNamesHelpButton_ = new QPushButton(groupBox4_);
	fileNamesHelpButton_->setObjectName(QString::fromUtf8("fileNamesHelpButton_"));
	sizePolicy.setHeightForWidth(fileNamesHelpButton_->sizePolicy().hasHeightForWidth());
	fileNamesHelpButton_->setSizePolicy(sizePolicy);

	horizontalLayout_7->addWidget(fileNamesHelpButton_);


	verticalLayout_7->addWidget(groupBox4_);

	verticalSpacer_ = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

	verticalLayout_7->addItem(verticalSpacer_);

	formLayout2_ = new QFormLayout();
	formLayout2_->setObjectName(QString::fromUtf8("formLayout2_"));
	formLayout2_->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
	fileNameLabel_ = new QLabel(tabFileName_);
	fileNameLabel_->setObjectName(QString::fromUtf8("fileNameLabel_"));

	formLayout2_->setWidget(0, QFormLayout::LabelRole, fileNameLabel_);

	fileNameText_ = new QLineEdit(tabFileName_);
	fileNameText_->setObjectName(QString::fromUtf8("fileNameText_"));

	formLayout2_->setWidget(0, QFormLayout::FieldRole, fileNameText_);

	fileNameSeparateSectionLabel_ = new QLabel(tabFileName_);
	fileNameSeparateSectionLabel_->setObjectName(QString::fromUtf8("fileNameSeparateSectionLabel_"));

	formLayout2_->setWidget(1, QFormLayout::LabelRole, fileNameSeparateSectionLabel_);

	fileNameSeparateSectionText_ = new QLineEdit(tabFileName_);
	fileNameSeparateSectionText_->setObjectName(QString::fromUtf8("fileNameSeparateSectionText_"));

	formLayout2_->setWidget(1, QFormLayout::FieldRole, fileNameSeparateSectionText_);


	verticalLayout_7->addLayout(formLayout2_);

	verticalSpacer2_ = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

	verticalLayout_7->addItem(verticalSpacer2_);

	addTab(tabFileName_, QString());

	setWindowTitle(QApplication::translate("AMExporterOptionGeneralAsciiView", "General Export Options", 0, QApplication::UnicodeUTF8));
	headerHelpText_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "This header is optionally included at the top of the file.  You can use special \"$keyword\" tags to add scan-specific informaton.", 0, QApplication::UnicodeUTF8));
	headerHelpButton_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Help", 0, QApplication::UnicodeUTF8));
	headerIncludeCheck_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Include Header", 0, QApplication::UnicodeUTF8));
	headerKeywordButton_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Keywords", 0, QApplication::UnicodeUTF8));
	setTabText(indexOf(tabHeader_), QApplication::translate("AMExporterOptionGeneralAsciiView", "Header", 0, QApplication::UnicodeUTF8));
	sourcesHelpText_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Here you can choose which data sets are included in the output.", 0, QApplication::UnicodeUTF8));
	sourcesHelpButton_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Help", 0, QApplication::UnicodeUTF8));
	includeAllSourcesCheck_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Include all data sets found in each scan", 0, QApplication::UnicodeUTF8));
	firstColumnOnlyCheck_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Use X-axis for first data source only", 0, QApplication::UnicodeUTF8));
	separateHigherDimensionSourcesCheck_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "2D and higher data sources into separate files", 0, QApplication::UnicodeUTF8));
	higherDimensionsInRows_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Separate files in rows or columns", 0, QApplication::UnicodeUTF8));
	detectedSourcesLabel_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Detected", 0, QApplication::UnicodeUTF8));
	includedSourcesLabel_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Included", 0, QApplication::UnicodeUTF8));
	sourcesAddButton_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "...", 0, QApplication::UnicodeUTF8));
	sourcesRemoveButton_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "...", 0, QApplication::UnicodeUTF8));
	sourcesAddExtraButton_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "+", 0, QApplication::UnicodeUTF8));
	sourcesDetailsBox_->setTitle(QApplication::translate("AMExporterOptionGeneralAsciiView", "Details", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
	sourcesRequiredCheck_->setToolTip(QApplication::translate("AMExporterOptionGeneralAsciiView", "Check if there should be an error if this data source doesn't exist in the scan.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
	sourcesRequiredCheck_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Required", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
	sourcesOmitAxisValuesCheck_->setToolTip(QApplication::translate("AMExporterOptionGeneralAsciiView", "By default, we add a column with the independent values (X-axis values) next to each column of data. Check this to omit that column (for example, if you know it's the same as the previous data source.)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
	sourcesOmitAxisValuesCheck_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Omit X-axis column", 0, QApplication::UnicodeUTF8));
	sourcesLocationBox_->setTitle(QApplication::translate("AMExporterOptionGeneralAsciiView", "Location", 0, QApplication::UnicodeUTF8));
	sourcesMainTableButton_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Main Table", 0, QApplication::UnicodeUTF8));
	sourcesSeparateSectionButton_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Separate Section", 0, QApplication::UnicodeUTF8));
	sourcesSeparateFileButton_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Separate File", 0, QApplication::UnicodeUTF8));
	setTabText(indexOf(tabDataSources_), QApplication::translate("AMExporterOptionGeneralAsciiView", "Data Sets", 0, QApplication::UnicodeUTF8));
	layoutHelpText_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Here you can choose how columns and sections are layed out and titled.", 0, QApplication::UnicodeUTF8));
	layoutHelpButton_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Help", 0, QApplication::UnicodeUTF8));
	columnHeaderCheck_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Column Header", 0, QApplication::UnicodeUTF8));
	columnHeaderHelpLabel_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Lucida Grande'; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">If checked, this title will used above each column in the table. You can use special &quot;$keyword&quot; tags here. <a href=\"columnHeaderHelp\"><span style=\" text-decoration: underline; color:#0000ff;\">More details...</span></a></p></body></html>", 0, QApplication::UnicodeUTF8));
	sectionHeaderCheck_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Section  Header", 0, QApplication::UnicodeUTF8));
	sectionHeaderHelpText_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Lucida Grande';  font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">If you've placed any data sets into their own separate section, this header will be placed above each section. You can use special &quot;$keyword&quot; tags here. <a href=\"sectionHeaderHelp\"><span style=\" text-decoration: underline; color:#0000ff;\">More details...</span></a></p></body></html>", 0, QApplication::UnicodeUTF8));
	columnSeparatorLabel_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Column Separator", 0, QApplication::UnicodeUTF8));
	columnSeparatorTabButton_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Tab", 0, QApplication::UnicodeUTF8));
	columnSeparatorSpaceButton_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Space", 0, QApplication::UnicodeUTF8));
	columnSeparatorCommaButton_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Comma", 0, QApplication::UnicodeUTF8));
	columnSeparatorSemicolonButton_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Semicolon", 0, QApplication::UnicodeUTF8));
	columnSeparatorOtherButton_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Other:", 0, QApplication::UnicodeUTF8));
	columnSeparatorOtherText_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "|", 0, QApplication::UnicodeUTF8));
	endOfLineSeparatorLabel_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "End of line separator", 0, QApplication::UnicodeUTF8));
	newlineCRLFCheck_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Window (CR LF)", 0, QApplication::UnicodeUTF8));
	newlineLFCheck_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Unix (LF)", 0, QApplication::UnicodeUTF8));
	newlineOtherCheck_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Other:", 0, QApplication::UnicodeUTF8));
	newlineOtherText_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "[eol]", 0, QApplication::UnicodeUTF8));
	setTabText(indexOf(tabLayout_), QApplication::translate("AMExporterOptionGeneralAsciiView", "Layout", 0, QApplication::UnicodeUTF8));
	fileNamesHelpText_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "The output file name can also include special \"$keyword\" tags.\n"
"If you've chosen to put any data sets into a separate file, provide the pattern for those file names here too.", 0, QApplication::UnicodeUTF8));
	fileNamesHelpButton_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Help", 0, QApplication::UnicodeUTF8));
	fileNameLabel_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Output File Name", 0, QApplication::UnicodeUTF8));
	fileNameSeparateSectionLabel_->setText(QApplication::translate("AMExporterOptionGeneralAsciiView", "Separate Section File Name", 0, QApplication::UnicodeUTF8));
	setTabText(indexOf(tabFileName_), QApplication::translate("AMExporterOptionGeneralAsciiView", "File Names", 0, QApplication::UnicodeUTF8));

	setCurrentIndex(3);
	option_ = option;

	dataSourceLocationGroup_ = new QButtonGroup(this);
	dataSourceLocationGroup_->addButton(sourcesMainTableButton_, AMExporterOptionGeneralAscii::CombineInColumnsMode);
	dataSourceLocationGroup_->addButton(sourcesSeparateSectionButton_, AMExporterOptionGeneralAscii::SeparateSectionsMode);
	dataSourceLocationGroup_->addButton(sourcesSeparateFileButton_, AMExporterOptionGeneralAscii::SeparateFilesMode);

	columnSeparatorGroup_ = new QButtonGroup(this);
	columnSeparatorGroup_->addButton(columnSeparatorTabButton_, 0);
	columnSeparatorGroup_->addButton(columnSeparatorSpaceButton_, 1);
	columnSeparatorGroup_->addButton(columnSeparatorCommaButton_, 2);
	columnSeparatorGroup_->addButton(columnSeparatorSemicolonButton_, 3);
	columnSeparatorGroup_->addButton(columnSeparatorOtherButton_, 4);

	newlineGroup_ = new QButtonGroup(this);
	newlineGroup_->addButton(newlineCRLFCheck_, 0);
	newlineGroup_->addButton(newlineLFCheck_, 1);
	newlineGroup_->addButton(newlineOtherCheck_, 2);

	if(availableDataSourcesModel)
		sourcesAvailableListView_->setModel(availableDataSourcesModel);


	initializeViewFromOption();

	makeUIConnections();
	makeOptionConnections();
}

AMExporterOptionGeneralAsciiView::~AMExporterOptionGeneralAsciiView()
{
}

void AMExporterOptionGeneralAsciiView::initializeViewFromOption()
{
	headerText_->setPlainText(option_->headerText());
	headerIncludeCheck_->setChecked(option_->headerIncluded());
	headerText_->setEnabled(option_->headerIncluded());

	sourcesIncludedListWidget_->clear();
	QStringList dataSources = option_->dataSources();
	foreach(QString ds, dataSources)
		sourcesIncludedListWidget_->addItem(ds);
	sourcesIncludedListWidget_->clearSelection();

	sourcesDetailsBox_->setDisabled(true);
	sourcesLocationBox_->setDisabled(true);

	includeAllSourcesCheck_->setChecked(option_->includeAllDataSources());
	firstColumnOnlyCheck_->setChecked(option_->firstColumnOnly());
	separateHigherDimensionSourcesCheck_->setChecked(option_->separateHigherDimensionalSources());
	sourcesSelectorFrame_->setEnabled(!option_->includeAllDataSources());
	higherDimensionsInRows_->setChecked(option_->higherDimensionsInRows());


	columnHeaderText_->setText(option_->columnHeader());
	columnHeaderCheck_->setChecked(option_->columnHeaderIncluded());
	columnHeaderText_->setEnabled(option_->columnHeaderIncluded());

	sectionHeaderText_->setPlainText(option_->sectionHeader());
	sectionHeaderCheck_->setChecked(option_->sectionHeaderIncluded());
	sectionHeaderText_->setEnabled(option_->sectionHeaderIncluded());


	columnSeparatorOtherText_->setDisabled(true);
	QString columnDelimiter = option_->columnDelimiter();
	if(columnDelimiter == "\t")
		columnSeparatorTabButton_->setChecked(true);
	else if(columnDelimiter == " ")
		columnSeparatorSpaceButton_->setChecked(true);
	else if(columnDelimiter == ",")
		columnSeparatorCommaButton_->setChecked(true);
	else if(columnDelimiter == ";")
		columnSeparatorSemicolonButton_->setChecked(true);
	else {
		columnSeparatorOtherButton_->setChecked(true);
		columnSeparatorOtherText_->setText(columnDelimiter);
		columnSeparatorOtherText_->setEnabled(true);
	}


	newlineOtherText_->setDisabled(true);
	QString newlineString = option_->newlineDelimiter();
	if(newlineString == "\r\n")
		newlineCRLFCheck_->setChecked(true);
	else if(newlineString == "\n")
		newlineLFCheck_->setChecked(true);
	else {
		newlineOtherCheck_->setChecked(true);
		newlineOtherText_->setText(newlineString);
		newlineOtherText_->setEnabled(true);
	}


	fileNameText_->setText(option_->fileName());
	fileNameSeparateSectionText_->setText(option_->separateSectionFileName());
}

void AMExporterOptionGeneralAsciiView::makeUIConnections()
{
	connect(headerIncludeCheck_, SIGNAL(clicked(bool)), headerText_, SLOT(setEnabled(bool)));
	connect(headerText_, SIGNAL(textChanged()), this, SLOT(onHeaderTextChanged()));

	connect(includeAllSourcesCheck_, SIGNAL(clicked(bool)), sourcesSelectorFrame_, SLOT(setDisabled(bool)));
	connect(includeAllSourcesCheck_, SIGNAL(toggled(bool)), firstColumnOnlyCheck_, SLOT(setEnabled(bool)));
	connect(includeAllSourcesCheck_, SIGNAL(toggled(bool)), separateHigherDimensionSourcesCheck_, SLOT(setEnabled(bool)));

	connect(sourcesAddButton_, SIGNAL(clicked()), this, SLOT(onSourcesAddButtonClicked()));
	connect(sourcesRemoveButton_, SIGNAL(clicked()), this, SLOT(onSourcesRemoveButtonClicked()));
	connect(sourcesAddExtraButton_, SIGNAL(clicked()), this, SLOT(onSourcesAddExtraButtonClicked()));
	connect(sourcesIncludedListWidget_, SIGNAL(currentRowChanged(int)), this, SLOT(onSourcesSelectedChanged(int)));


	connect(sourcesRequiredCheck_, SIGNAL(clicked(bool)), this, SLOT(onSourcesRequiredClicked(bool)));
	connect(sourcesOmitAxisValuesCheck_, SIGNAL(clicked(bool)), this, SLOT(onSourcesOmitAxisValuesClicked(bool)));
	connect(dataSourceLocationGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onSourcesLocationChanged(int)));

	connect(columnHeaderCheck_, SIGNAL(clicked(bool)), columnHeaderText_, SLOT(setEnabled(bool)));
	connect(sectionHeaderCheck_, SIGNAL(clicked(bool)), sectionHeaderText_, SLOT(setEnabled(bool)));
	connect(sectionHeaderText_, SIGNAL(textChanged()), this, SLOT(onSectionHeaderTextChanged()));

	connect(columnSeparatorGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onColumnSeparatorChanged(int)));
	connect(columnSeparatorOtherText_, SIGNAL(textChanged(QString)), this, SLOT(onColumnSeparatorOtherTextChanged(QString)));
	connect(newlineGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onNewlineButtonChanged(int)));
	connect(newlineOtherText_, SIGNAL(textChanged(QString)), this, SLOT(onNewlineOtherTextChanged(QString)));

	connect(higherDimensionsInRows_, SIGNAL(toggled(bool)), this, SLOT(onHigherDimensionsInRowsToggled(bool)));

	connect(sourcesAvailableListView_->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(onSourcesAvailableSelectedChanged()));
}


void AMExporterOptionGeneralAsciiView::makeOptionConnections() {

	connect(headerIncludeCheck_, SIGNAL(clicked(bool)), option_, SLOT(setHeaderIncluded(bool)));

	connect(columnHeaderText_, SIGNAL(textChanged(QString)), option_, SLOT(setColumnHeader(QString)));
	connect(columnHeaderCheck_, SIGNAL(clicked(bool)), option_, SLOT(setColumnHeaderIncluded(bool)));

	connect(sectionHeaderCheck_, SIGNAL(clicked(bool)), option_, SLOT(setSectionHeaderIncluded(bool)));

	connect(fileNameText_, SIGNAL(textChanged(QString)), option_, SLOT(setFileName(QString)));
	connect(fileNameSeparateSectionText_, SIGNAL(textChanged(QString)), option_, SLOT(setSeparateSectionFileName(QString)));

	connect(includeAllSourcesCheck_, SIGNAL(clicked(bool)), option_, SLOT(setIncludeAllDataSources(bool)));

	connect(firstColumnOnlyCheck_, SIGNAL(toggled(bool)), option_, SLOT(setFirstColumnOnly(bool)));

	connect(separateHigherDimensionSourcesCheck_, SIGNAL(toggled(bool)), option_, SLOT(setSeparateHigherDimensionalSources(bool)));
}


// disable add button when none selected, enable when one is.
void AMExporterOptionGeneralAsciiView::onSourcesAvailableSelectedChanged() {

	if(sourcesAvailableListView_->selectionModel()->selectedIndexes().count() > 0)
		sourcesAddButton_->setEnabled(true);
	else
		sourcesAddButton_->setEnabled(false);
}

// add source
void AMExporterOptionGeneralAsciiView::onSourcesAddButtonClicked() {

	foreach(QModelIndex index, sourcesAvailableListView_->selectionModel()->selectedIndexes()) {
		if(index.column() == 0) {
			QString newSourceName =  index.data(AM::NameRole).toString();
			option_->addDataSource(newSourceName, false);
			sourcesIncludedListWidget_->addItem(newSourceName);
		}
	}

	// question... does this fire up onSourcesSelectedChanged() automatically?
}

// remove source
void AMExporterOptionGeneralAsciiView::onSourcesRemoveButtonClicked() {
	int removeAtIndex = sourcesIncludedListWidget_->currentRow();
	if(removeAtIndex == -1)
		return;

	int newSelectedIndex = removeAtIndex - 1;
	if(newSelectedIndex == -1 && sourcesIncludedListWidget_->count() > 1)
		newSelectedIndex = 0;

	// block signals so we don't trigger onSourcesSelectedChanged()
	sourcesIncludedListWidget_->blockSignals(true);

	sourcesIncludedListWidget_->takeItem(removeAtIndex);
	option_->removeDataSourceAt(removeAtIndex);
	sourcesIncludedListWidget_->blockSignals(false);
	sourcesIncludedListWidget_->setCurrentRow(newSelectedIndex, QItemSelectionModel::ClearAndSelect);
	// check that onSourcesSelectedChanged happens here? Nope... Need to call manually.
	onSourcesSelectedChanged(newSelectedIndex);
}

// dialog prompt and add source
void AMExporterOptionGeneralAsciiView::onSourcesAddExtraButtonClicked() {
	QString customSourceName = QInputDialog::getText(this, "Add custom data set", "Include a custom data set named:");
	if(!customSourceName.isEmpty()) {
		option_->addDataSource(customSourceName, false, AMExporterOptionGeneral::CombineInColumnsMode, false);
		sourcesIncludedListWidget_->addItem(customSourceName);
	}
}

// if index valid: enable edit boxes, block signals and set info boxes to match source, re-enable signals, enable remove button.  If not valid: disable remove button, disable edit boxes
void AMExporterOptionGeneralAsciiView::onSourcesSelectedChanged(int index) {

	if(index == -1) {
		sourcesRemoveButton_->setEnabled(false);
		sourcesLocationBox_->setEnabled(false);
		sourcesDetailsBox_->setEnabled(false);
	}
	else {
		sourcesRemoveButton_->setEnabled(true);
		sourcesLocationBox_->setEnabled(true);
		sourcesDetailsBox_->setEnabled(true);

		sourcesRequiredCheck_->blockSignals(true);
		sourcesOmitAxisValuesCheck_->blockSignals(true);
		dataSourceLocationGroup_->blockSignals(true);

		sourcesRequiredCheck_->setChecked(option_->isDataSourceRequired(index));
		sourcesOmitAxisValuesCheck_->setChecked(option_->isDataSourceAxisValueColumnOmitted(index));
		dataSourceLocationGroup_->button(option_->dataSourceOrganizeMode(index))->setChecked(true);

		sourcesRequiredCheck_->blockSignals(false);
		sourcesOmitAxisValuesCheck_->blockSignals(false);
		dataSourceLocationGroup_->blockSignals(false);
	}
}

// if source selected, set its required state
void AMExporterOptionGeneralAsciiView::onSourcesRequiredClicked(bool isRequired) {

	int index = sourcesIncludedListWidget_->currentRow();
	if(index == -1)
		return;

	option_->setDataSourceRequired(index, isRequired);
}

// if source selected, set its omit state
void AMExporterOptionGeneralAsciiView::onSourcesOmitAxisValuesClicked(bool isOmitted) {
	int index = sourcesIncludedListWidget_->currentRow();
	if(index == -1)
		return;

	option_->setDataSourceAxisValueColumnOmitted(index, isOmitted);
}

// if source selected, set source's organize mode to match locationMode
void AMExporterOptionGeneralAsciiView::onSourcesLocationChanged(int locationMode) {
	int index = sourcesIncludedListWidget_->currentRow();
	if(index == -1)
		return;

	option_->setDataSourceOrganizeMode(index, locationMode);
}

// if separatorButton is not other: set option's separator, disable otherBox. If separatorButton is other, enable otherBox, set option's separator to text of otherBox, selectAll in otherBox.
void AMExporterOptionGeneralAsciiView::onColumnSeparatorChanged(int separatorButton) {

	QString separator;

	switch(separatorButton) {

	case 0:
		separator = "\t";
		break;

	case 1:
		separator = " ";
		break;

	case 2:
		separator = ",";
		break;

	case 3:
		separator = ";";
		break;

	case 4:
		separator = columnSeparatorOtherText_->text();	/// \todo Handle escape chars?
		break;
	}

	option_->setColumnDelimiter(separator);

	if(separatorButton == 4) {
		columnSeparatorOtherText_->setEnabled(true);
		columnSeparatorOtherText_->setFocus();
		columnSeparatorOtherText_->selectAll();	// hint for the user to edit this.
	}
	else {
		columnSeparatorOtherText_->setEnabled(false);
	}
}

// if "other" mode selected: set option's separator to otherText
void AMExporterOptionGeneralAsciiView::onColumnSeparatorOtherTextChanged(const QString& otherText) {
	if(columnSeparatorOtherButton_->isChecked()) {
		option_->setColumnDelimiter(otherText); /// \todo Handle escape characters?
	}
}

// if newlineButton is not "other": set option's newline, disable otherBox.  If newlineButton is "other", enable otherBox, set option's newline to text of otherBox, selectAll in otherBox.
void AMExporterOptionGeneralAsciiView::onNewlineButtonChanged(int newlineButton) {
	QString newline;

	switch(newlineButton) {
	case 0:
		newline = "\r\n";
		break;
	case 1:
		newline = "\n";
		break;
	case 2:
		newline = newlineOtherText_->text();	/// \todo Handle escape chars?
		break;
	}

	option_->setNewlineDelimiter(newline);

	if(newlineButton == 2) {
		newlineOtherText_->setEnabled(true);
		newlineOtherText_->setFocus();
		newlineOtherText_->selectAll();
	}
	else {
		newlineOtherText_->setEnabled(false);
	}
}

// if "other" mode selected: set option's newline to otherText.
void AMExporterOptionGeneralAsciiView::onNewlineOtherTextChanged(const QString& otherText) {
	if(newlineOtherCheck_->isChecked()) {
		option_->setNewlineDelimiter(otherText); /// \todo Handle escape chars?
	}
}

void AMExporterOptionGeneralAsciiView::onHeaderTextChanged()
{
	option_->setHeaderText(headerText_->toPlainText());
}

void AMExporterOptionGeneralAsciiView::onSectionHeaderTextChanged()
{
	option_->setSectionHeader(sectionHeaderText_->toPlainText());
}

void AMExporterOptionGeneralAsciiView::onHigherDimensionsInRowsToggled(bool checked)
{
	option_->setHigherDimensionsInRows(checked);
}
