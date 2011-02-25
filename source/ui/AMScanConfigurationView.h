/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#ifndef AMSCANCONFIGURATIONVIEW_H
#define AMSCANCONFIGURATIONVIEW_H

#include <QWidget>
#include <QRadioButton>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

class AMScanConfiguration;

/// If you want your scan configuration widget to easily be able to start scans, you should inherit from AMScanConfigurationView, and install it within an AMScanConfigurationHolder.
class AMScanConfigurationView : public QWidget {
	Q_OBJECT

public:
	/// Default constructor
	explicit AMScanConfigurationView(QWidget* parent = 0) : QWidget(parent) {}

	/// This function must return a pointer to the scan configuration, that is being configure within your view.
	/*! Any functions that use this will make a copy of the configuration immediately, so you don't need to worry about making changes to the pointed-to object.*/
	virtual const AMScanConfiguration* configuration() const = 0;
};


class AMScanConfigurationScanDirector;

/// This widget holds a scan configuration widget, and provides user controls to start the configured scan within the workflow.  It can be constructed to surround any AMScanConfigurationView. Below the widget, it adds buttons to start the configured scan or add it to the queue.
/*! This widget takes ownership of the view specifified in its constructor.

When the "Start Scan" button or "Add to Workflow" button is pressed, a copy of the scan configuration view's current scan configuration is made, and this is what gets added to the queue.

\todo Consider using a workflow API instead of all these signals and slots that must be hoooked up to communicate with the workflow.
Justification: you've decoupled it from the workflow completely, by sending a whole API through signals and slots.  Yes, decoupling good, but the result is a crapton of signals that need to be connected (for ex: in AMAppController / SGMAppController), so this requires a lot of code to be added for each holder instance, and is prone to bugs if the user doesn't connect it all.  With this much detail in those signals and slots... is there any way you would use the holder WITHOUT a workflow manager (ie: with another object?).  If not, consider coupling the two, and just calling the workflow functions.  Simplifies the holder code, and simplifies the usage of it.  (External users have to write much less code.)  More coupling, theoretically, but less encapsulation/work done for them.
*/
class AMScanConfigurationHolder : public QWidget
{
	Q_OBJECT
public:
	/// Construct a holder for an AMScanConfigurationView \c view or 0 if no view is ready yet.
	AMScanConfigurationHolder(AMScanConfigurationView* view = 0, QWidget *parent = 0);
	~AMScanConfigurationHolder();

	/// Set the \c view contained within the holder.  If there is an existing view, the old view is deleted.
	/*! You can pass in 0 to remove the existing view from the holder.*/
	void setView(AMScanConfigurationView* view);

public slots:
	virtual void onFreeToScan(bool queueEmpty, bool queueNotRunning);
	virtual void onLockdownScanning(bool isLocked, QString reason);
	virtual void onAddedToQueue(AMScanConfiguration *configuration);

	/// This slot will be used to notify this widget when it becomes the current (active) widget
	virtual void onBecameCurrentWidget() {}

signals:
	void addToQueueRequested(AMScanConfiguration *configuration, bool startNow = false);
	void cancelAddToQueueRequest();
	void goToQueueRequested();
	void lockdownScanning(bool isLocked, QString reason);

protected slots:

	virtual void onStartScanRequested();
	virtual void onAddToQueueRequested();

	virtual void goToQueue();
	virtual void goToNewScan();

protected:
	/// This is the scan configuration we've last attempted to add to the workflow.
	AMScanConfiguration* configuration_;

	bool requestedStart_;
	bool canStartImmediately_;

	/// UI elements
	QPushButton* startScanButton_, *addToQueueButton_;
	QRadioButton* goToWorkflowOption_, *setupAnotherScanOption_;
	QLabel* statusLabel_;
	QVBoxLayout* layout_;

	/// This is the scan configuration widget we're wrapping
	AMScanConfigurationView* view_;


	AMScanConfigurationScanDirector *scanDirector_;
};

#include <QBoxLayout>
#include <QGridLayout>
#include <QCheckBox>
class AMScanConfigurationScanDirector : public QWidget
{
Q_OBJECT
public:
	AMScanConfigurationScanDirector(QWidget *parent = 0);

public slots:
	void showDirector(const QString& reason);

signals:
	void cancel();
	void startNow();
	void addToQueue(int index);

protected slots:
	void onWaitNext();
	void onAppend();

	void onAlwaysStartNow(bool checked);
	void onAlwaysWaitNext(bool checked);
	void onAlwaysAppend(bool checked);
	void onAlwaysCancel(bool checked);

protected:
	QString message_;
	bool alwaysStartNow_;
	bool alwaysWaitNext_;
	bool alwaysAppend_;
	bool alwaysCancel_;

	QVBoxLayout *vl_;
	QLabel *messageLabel_;
	QGridLayout *gl_;
	QPushButton *startNowButton_;
	QPushButton *waitNextButton_;
	QPushButton *appendButton_;
	QPushButton *cancelButton_;
	QLabel *startNowLabel_;
	QLabel *waitNextLabel_;
	QLabel *appendLabel_;
	QLabel *cancelLabel_;
	QLabel *setDefaultLabel_;
	QCheckBox *startNowCheck_;
	QCheckBox *waitNextCheck_;
	QCheckBox *appendCheck_;
	QCheckBox *cancelCheck_;
};

/*
class AMFastScanConfigurationHolder : public AMScanConfigurationHolder
{
	Q_OBJECT
public:
	AMFastScanConfigurationHolder(QWidget *parent = 0);
	~AMFastScanConfigurationHolder();

public slots:
	/// This slot will be used to notify this widget when it becomes the current (active) widget
	void onBecameCurrentWidget();

protected slots:
	void createScanConfiguration();
	void destroyScanConfigurationViewer();

	void setAutoSavePath(const QString& autoSavePath);
	void setLastSettings(SGMFastScanParameters *lastSettings);

private:
	SGMFastScanConfiguration* cfg() { return qobject_cast<SGMFastScanConfiguration*>(cfg_);}

protected:
	SGMFastScanConfigurationViewer *sfscViewer_;

	QString autoSavePath_;
	SGMFastScanParameters *lastSettings_;

	QVBoxLayout *vl_;
};
*/

#endif // AMSCANCONFIGURATIONVIEW_H
