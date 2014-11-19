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


#ifndef AMIMPORTCONTROLLERWIDGET_H
#define AMIMPORTCONTROLLERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QProgressBar>
#include <QLabel>
#include <QComboBox>
#include <QGridLayout>
#include <QCheckBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDateTimeEdit>
#include <QPushButton>
#include <QApplication>

#include "ui/AMThumbnailScrollViewer.h"
#include "ui/dataman/AMRunSelector.h"

class AMImportControllerWidget : public QWidget {
public:
 	virtual ~AMImportControllerWidget(){}
	explicit AMImportControllerWidget(QWidget* parent = 0);

	AMThumbnailScrollWidget* thumbnailViewer();
	AMRunSelector* runEdit();

	QProgressBar* progressBar();

	void setName(const QString& name);
	void setNumber(int number);
	void setDateTime(QDateTime dateTime);
	void setProgressLabel(const QString& progress);
	void setLoadingStatus(const QString& status);
	void addFormat(const QString& format);

	QString name();
	int number();
	QDateTime dateTime();
	int sampleId();
	QString format();
	int selectedFormatIndex();

	bool isIncludeNameChecked();
	bool isIncludeNumberChecked();
	bool isIncludeDataTimeChecked();
	// UI elements
signals:
	void nextClicked();
	void applyAllClicked();
	void cancelClicked();
	void selectedFormatChanged(int);
	void includeNameChanged(bool);
	void includeNumberChanged(bool);
	void includeDateTimeChanged(bool);

protected:
	AMThumbnailScrollWidget* thumbnailViewer_;
	AMRunSelector* runEdit_;

	QVBoxLayout *verticalLayout_3;
	QFrame *AMImportControllerWidgetFrame1;
	QHBoxLayout *horizontalLayout_2;
	QVBoxLayout *verticalLayoutLeft;
	QSpacerItem *verticalSpacer_2;
	QProgressBar *progressBar_;
	QLabel *progressLabel_;
	QVBoxLayout *verticalLayoutRight;
	QHBoxLayout *horizontalLayout;
	QLabel *formatLabel_;
	QComboBox *formatComboBox_;
	QLabel *loadingStatusLabel_;
	QSpacerItem *verticalSpacer;
	QLabel *checkInstructionLabel_;
	QGridLayout *gridLayout;
	QCheckBox *checkName_;
	QLabel *nameLabel_;
	QLineEdit *nameEdit_;
	QCheckBox *checkNumber_;
	QLabel *numberLabel_;
	QSpinBox *numberEdit_;
	QCheckBox *checkDateTime_;
	QLabel *dateTimeLabel_;
	QDateTimeEdit *dateTimeEdit_;
	QCheckBox *checkRun_;
	QLabel *runLabel_;
	QCheckBox *checkSample_;
	QLabel *sampleLabel_;
	QSpinBox *sampleEdit_;
	QHBoxLayout *horizontalLayout_3;
	QPushButton *cancelButton_;
	QSpacerItem *horizontalSpacer;
	QPushButton *nextButton_;
	QPushButton *applyAllButton_;
protected slots:
	void onNameIncludeCheckboxChecked(bool);
	void onNumberIncludeCheckboxChecked(bool);
	void onDateTimeIncludeCheckboxChanged(bool);

private:
	void setupUi();
	void connectSignals();
};

#endif // AMIMPORTCONTROLLERWIDGET_H
