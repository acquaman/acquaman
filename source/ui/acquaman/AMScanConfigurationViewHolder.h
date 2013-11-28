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


#ifndef AMSCANCONFIGURATIONVIEWHOLDER_H
#define AMSCANCONFIGURATIONVIEWHOLDER_H

//#include <QWidget>

//class QRadioButton;
//class QPushButton;
//class QLabel;
//class QCheckBox;
//class QVBoxLayout;

//class AMScanConfiguration;
//class AMScanConfigurationView;
//class AMWorkflowManagerView;

//#include "dataman/AMScanExemplar.h"

//class AMScanExemplarDictionary;
//class AMDictionaryLineEdit;

///// This widget holds a scan configuration widget, and provides user controls to start the configured scan within the workflow.  It can be constructed to surround any AMScanConfigurationView. Below the widget, it adds buttons to start the configured scan or add it to the queue.
///*! This widget takes ownership of the view specifified in its constructor.

//When the "Start Scan" button or "Add to Workflow" button is pressed, a copy of the scan configuration view's current scan configuration is made, and this is what gets added to the queue.
//*/
//class AMScanConfigurationViewHolder : public QWidget
//{
//	Q_OBJECT
//public:
//	/// Construct a holder for an AMScanConfigurationView \c view (or 0 if no view is ready yet).  A pointer to the workflow manager \c workflow must be provided, which we use to add scan actions when required.
//	/*! If provided, this widget takes ownership of the \c view */
//	AMScanConfigurationViewHolder(AMWorkflowManagerView* workflow, AMScanConfigurationView* view = 0, QWidget *parent = 0);
//	/// Destructor.
//	~AMScanConfigurationViewHolder();

//	/// Set the \c view contained within the holder.  If there is an existing view, the old view is deleted.
//	/*! You can pass in 0 to remove the existing view from the holder.*/
//	void setView(AMScanConfigurationView* view);

//public slots:

//signals:

//	/// Emitted to request the main application controller show the workflow pane.
//	void showWorkflowRequested();

//protected slots:

//	/// Called when the "Start Scan" button is clicked
//	virtual void onStartScanRequested();
//	/// Called when the "Add to Queue" button is clicked.
//	virtual void onAddToQueueRequested();

//	/// This re-evaluates the state of the workflow and provides user feedback on the "Start Scan" button as required.
//	/*! If the scan configuration view or scan configuration are invalid, sets the button disabled with a message. If the workflow cannot be started because the beamline is busy (ie: one of your scans, or someone else's scan, is running), sets the button disabled with a message. Otherwise enables the button. */
//	void reviewStartScanButtonState();

//	/*
//	void onDoExportNameCheckBoxStatedChanged(int state);
//	*/

//protected:

//	/// UI elements
//	/*
//	QLabel *scanNameLabel_;
//	AMDictionaryLineEdit *scanNameDictionaryLineEdit_;
//	QLabel *scanNameExampleLabel_;

//	QLabel *autoExportLabel_;
//	QCheckBox *doExportNameCheckBox_;
//	QCheckBox *doAutoExportCheckBox_;

//	QLabel *exportNameLabel_;
//	AMDictionaryLineEdit *exportNameDictionaryLineEdit_;
//	QLabel *exportNameExampleLabel_;
//	*/

//	QLabel *whenDoneLabel_;
//	QPushButton* startScanButton_, *addToQueueButton_;
//	QRadioButton* goToWorkflowOption_, *setupAnotherScanOption_;
//	QVBoxLayout* layout_;

//	/// This is the scan configuration widget we're wrapping
//	AMScanConfigurationView* view_;
//	/// This is a pointer to the workflow manager (view, for now). We use it to add scan actions to the workflow
//	AMWorkflowManagerView* workflow_;

//	/*
//	AMScanExemplar testExemplar_;
//	AMScanExemplarDictionary *exemplarNameDictionary_;
//	AMScanExemplarDictionary *exemplarExportNameDictionary_;
//	*/
//};

#endif // AMSCANCONFIGURATIONVIEWHOLDER_H
