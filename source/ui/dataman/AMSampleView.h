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


#ifndef AMSAMPLEVIEW_H
#define AMSAMPLEVIEW_H

#include <QWidget>

class QLineEdit;
class QHBoxLayout;
class QVBoxLayout;
class QPushButton;
class QCompleter;
class QStringListModel;
class QComboBox;
class QTextEdit;
class QLayoutItem;

class AMSample;
class AMElementListEdit;
class AMPeriodicTableDialog;
class AMShapeDataView;

#define AMSAMPLEVIEW_SAVE_FAILED 604001

class AMSampleReadOnlyView : public QWidget{
public:
	/// Simple constructor takes a const pointer to an AMSample
	AMSampleReadOnlyView(const AMSample *sample, QWidget *parent = 0);

protected:
	/// Called to initialize the data in the widgets
	virtual void initializeSampleViewData();
	/// Loads all of the possible tags from the database
	void loadTagsFromDatabase();

protected:
	/// Const pointer to the AMSample we are viewing
	const AMSample *constSample_;

	/// Text box showing name
	QLineEdit* nameText_;
	/// Text box showing dateTime
	QLineEdit* dateTimeText_;
	/// Text box showing notes
	QTextEdit* notesText_;
	/// text box current tag
	QLineEdit* tagText_;
	/// the combobox showing all the current samples tags
	QComboBox* tagBox_;
	/// completer for autocompleting tags
	QCompleter* completer_;
	/// list of all previously used tags, used for autocomplete
	QStringListModel* wordList_;
	/// text box showing elements
	QLineEdit* elementsText_;
	/// text box showing the sample plate name
	QLineEdit* samplePlateName_;

	/// Master layout
	QVBoxLayout* sampleViewLayout_;
	/// LayoutItem to help subclasses place other tag widgets properly
	QLayoutItem *tagLayoutItem_;
	/// LayoutItem to help subclasses place other element widgets properly
	QLayoutItem *elementLayoutItem_;
};

class AMSampleAdvancedView;

/// This class is the view for an AMSample.
/// It can be used to load or save a sample to the database,
/// as well as to set or view the name, date, notes, tags, elements,
/// and sample plate of the sample.
class AMSampleView : public AMSampleReadOnlyView
{
Q_OBJECT
public:
	/// Simple constructor takes a pointer to an AMSample to view
	AMSampleView(AMSample* sample, QWidget* parent = 0);

signals:
	/// Used by AMSamplePlate view to automatically delete views
	void aboutToClose();

protected slots:
	/// Set the sample to view
	void setSample(AMSample* sample);
	/// Set the name of the current sample
	void setName(QString name);
	/// Set the notes of the current sample
	void setNotes();
	/// Add a tag to the current sample
	void addTag();
	/// Remove a tag from a specified sample
	void removeTag(int index);
	/// Remove a tag from the current sample
	void removeTag();
	/// Show the periodic table dialog box
	void showPeriodicTable();

	/// Calls setCurrentTag on the sample
	void setCurrentTag();
	/// Commented out at the moment
	void updateTags(QStringList);
	/// If our sample is removed, then close the window and delete ourselves later
	void onSampleAboutToBeRemoved();

	/// Handles clicks from the moreInfoButton to launch the advancedView (or show it, if we already have one)
	void onMoreInfoButtonClicked();

	/// Save the sample to the database
	virtual void saveToDb();

protected:
	/// Called to initialize the data in the widgets
	virtual void initializeSampleViewData();

	/// Reimplemented to make sure that aboutToClose is emitted before closing
	virtual void closeEvent(QCloseEvent *event);

protected:
	/// push button to bring up th element dialog
	QPushButton* showElementDialog_;
	/// the element dialog window
	AMPeriodicTableDialog* elementDialog_;

	/// push button used to remove tags from the current sample
	QPushButton* removeTagButton_;

	/// push button used to save the current sample to the database
	QPushButton* saveToDb_;

	/// the sample that is being viewed by this view
	AMSample* sample_;

	/// Push button to launch the advancedView
	QPushButton *moreInfoButton_;
	/// A pointer to an advancedView we'll create if the user requests it. It's our job to manage it.
	AMSampleAdvancedView *advancedView_;
};

class AMSampleAdvancedView : public AMSampleView
{
Q_OBJECT
public:
	/// Simple constructor takes a pointer to an AMSample, extends AMSampleView to include shapeData information in an AMShapeDataView
	AMSampleAdvancedView(AMSample *sample, QWidget *parent = 0);

protected slots:
	/// Handles updating when there are changes to the shapeData
	void onSampleShapeDataChanged();

protected:
	/// View for visualizing the sample's AMShapeData
	AMShapeDataView* shapeDataView_;
};

#endif // AMSAMPLEVIEW_H
