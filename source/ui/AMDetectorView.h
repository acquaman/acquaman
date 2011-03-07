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


#ifndef AMDETECTORVIEW_H
#define AMDETECTORVIEW_H

#include "AMControlSetView.h"
#include "beamline/AMDetector.h"
#include "ui/AMControlEditor.h"
#include "ui/AMDetectorInfoView.h"
#include "QMessageBox"
#include "QMetaMethod"

class AMDetectorView;


class AMDetectorViewObjectInfo{
public:
	/// The default constructor creates an invalid AMDbObject.
	AMDetectorViewObjectInfo() {
		viewMetaObject = 0;
	}

	/// fill the className
	AMDetectorViewObjectInfo(AMDetectorView *prototypeView, AMDetectorInfo *prototypeDetector);

	/// fill the className (This version doesn't require an instance. The \c classMetaObject can be retrieved statically with Class::staticMetaObject. )
	AMDetectorViewObjectInfo(const QMetaObject *classMetaObject, const QMetaObject *supportsMetaObject);


	/// Indicates this AMDbObjectInfo represents a valid object.
	bool isValid() const { return (viewMetaObject != 0); }

	QString viewClassName;	///< the class name (C++ type name) of the view objects
	QString detectorClassName; ///< the class name (C++ type name) of the detector objects
	const QMetaObject* viewMetaObject;	///< QMetaObject pointer with the complete class meta-information. The remaining values can be determined by parsing the metaObject's classInfo parameters, but are stored here for efficiency.
	const QMetaObject* detectorMetaObject; ///< QMetaObject pointer with complete class meta-information for the detector this view supports

private:
	/// used to implement both constructors
	void initWithMetaObject(const QMetaObject *classMetaObject, const QMetaObject *supportsMetaObject);

	/// checks to make sure a QMetaObject inherits AMDetectorView
	bool inheritsDetectorView(const QMetaObject *metaObject);

	/// checks to make sure a QMetaObject inherits AMDetectorInfo
	bool inheritsDetectorInfo(const QMetaObject *metaObject);
};

namespace AMDetectorViewSupport{
	extern QHash<QString, AMDetectorViewObjectInfo> registeredClasses_;

	// Registers the pairing of a view class (AMDetectorView descendant) and a detector class (AMDetectorInfo descendent)
	// Class T1 needs to inherit AMDetectorView and Class T2 needs to inherit AMDetectorInfo
	template <class Ta, class Tb>
			bool registerClass() {
		// create the meta object for the view
		const QMetaObject *viewMo = &(Ta::staticMetaObject);
		// create the meta object for the detector
		const QMetaObject *detectorMo = &(Tb::staticMetaObject);

		// is this a subclass of AMDetectorView? (Or an AMDetectorView itself?)
		const QMetaObject* viewSuperClass = viewMo;
		bool inheritsDetectorView;
		do {
			inheritsDetectorView = (viewSuperClass->className() == QString("AMDetectorView"));
		}
		while( (viewSuperClass=viewSuperClass->superClass()) && inheritsDetectorView == false );
		if(!inheritsDetectorView)
			return false;	// can't register a non AMDetectorView subclass.

		// is this a subclass of AMDetectorInfo? (Or an AMDetectorInfo itself?)
		const QMetaObject* detectorSuperClass = detectorMo;
		bool inheritsDetectorInfo;
		do {
			inheritsDetectorInfo = (detectorSuperClass->className() == QString("AMDetectorInfo"));
		}
		while( (detectorSuperClass=detectorSuperClass->superClass()) && inheritsDetectorInfo == false );
		if(!inheritsDetectorInfo)
			return false;	// can't register a non AMDetectorInfo subclass.

		// is it already registered? return true.
		QString className(viewMo->className());
		if(registeredClasses_.contains(className)) {
			//AMErrorMon::report(AMErrorReport(0, AMErrorReport::Debug, 0, QString("Detector View Support: The class '%1' has already been registered with detector view. Skipping duplicate registration.").arg(className)));
			return true;
		}

		AMDetectorViewObjectInfo newInfo(viewMo, detectorMo);

		registeredClasses_.insert(className, newInfo);
		return true;
	}

	/// returns a const pointer to the hash of registered AMDetectorViewObjectInfo classes
	const QHash<QString, AMDetectorViewObjectInfo>* registeredClasses();

	/// returns a const pointer to a list of AMDetectorViewObjectInfo classes supporting the AMDetector
	const QList<AMDetectorViewObjectInfo> supportedClasses(AMDetectorInfo *detector);

	/// Useful for gui generation, this creates the first view available from the supportedClasses for the AMDetector. You can use qobject_cast<>() or type() to find out the detailed type of the new object.  Returns 0 if no object found.
	/*! Ownership of the newly-created object becomes the responsibility of the caller. */
	AMDetectorView* createDetectorView(AMDetectorInfo *detector);
}

class AMDetectorView : public QWidget
{
Q_OBJECT
public:
	Q_INVOKABLE explicit AMDetectorView(QWidget *parent = 0);

	virtual AMDetectorInfo* detector();

protected:
	virtual bool setDetector(AMDetectorInfo *detector);
	friend AMDetectorView* AMDetectorViewSupport::createDetectorView(AMDetectorInfo *detector);

signals:

public slots:

};

class AMSingleControlDetectorView : public AMDetectorView
{
Q_OBJECT
public:
	Q_INVOKABLE explicit AMSingleControlDetectorView(AMSingleControlDetector *detector = 0, QWidget *parent = 0);

	AMDetectorInfo* detector();

protected:
	AMControlEditor *fbk_;
	QHBoxLayout *hl_;
	AMSingleControlDetector *detector_;

	bool setDetector(AMDetectorInfo *detector);
};

class MCPDetectorView : public AMDetectorView
{
Q_OBJECT
public:
	Q_INVOKABLE explicit MCPDetectorView(MCPDetector *detector = 0, QWidget *parent = 0);

	AMDetectorInfo* detector();

protected:
	AMControlEditor *fbk_;
	QHBoxLayout *hl_;
	MCPDetector *detector_;

	bool setDetector(AMDetectorInfo *detector);
};

class PGTDetectorView : public AMDetectorView
{
Q_OBJECT
public:
	Q_INVOKABLE explicit PGTDetectorView(PGTDetector *detector = 0, QWidget *parent = 0);

	AMDetectorInfo* detector();

protected:
	QLabel *controlLabel_;
	QHBoxLayout *hl_;
	PGTDetector *detector_;

	bool setDetector(AMDetectorInfo *detector);
};

class AMDetectorSetView : public QGroupBox
{
Q_OBJECT
public:
	/// Constructor
	/*! \param viewSet Pointer to the AMControlSet to view.
	  \param parent Pointer to QWidget to act as parent.
	  */
	explicit AMDetectorSetView(AMDetectorSet *viewSet, bool configureOnly = false, QWidget *parent = 0);

	AMDetectorView* boxByName(const QString &name){
		return detectorBoxes_.at(viewSet_->indexOfKey(name));
	}

	AMDetectorView* boxAt(int row){
		return detectorBoxes_.at(row);
	}

	/*
	/// Returns the current values of the control set
	AMControlInfoList currentValues();
	/// Returns the desired values if the view is used for configuration only (returns the current values if the view is not for configuration only)
	AMControlInfoList configValues();
	//QMap<QString, QVariant> configValues() { return configValues_;}
	//bool boxTrigger() const { return boxTrigger_;}
	*/

signals:
	/*
	//void configValuesChanged();
	/// Emitted when any control value in the control set changes (but ONLY when the view is not configure only)
	void currentValuesChanged(AMControlInfoList);
	/// Emitted when any configuration value changes (but ONLY when the view is configure only)
	void configValuesChanged(AMControlInfoList);
	*/

public slots:
	//void setConfigValues(QMap<QString, QVariant> configValues);
	//void resetBoxTrigger(){boxTrigger_ = false;}

protected slots:
	void onDetectorAddedToSet(int index);
	/*
	void onControlSetValuesChanged(AMControlInfoList infoList);
	void onConfigurationValueChanged();
	//void onBoxUpdate(const QString& value);
	*/

protected:
	/// Pointer to the AMControlSet which is the subject of this view.
	AMDetectorSet *viewSet_;
	bool configureOnly_;
	//QMap<QString, QVariant> configValues_;
	QList<AMDetectorView*> detectorBoxes_;
	//QList<bool> dirty_;
	//bool boxTrigger_;
	QVBoxLayout *vl_;
	QHBoxLayout *hl_;
};


class PGTOldDetectorView : public PGTOldDetectorInfoView
{
	Q_OBJECT
public:
	PGTOldDetectorView(PGTDetector *detector, AMDetectorInfo *configDetector = 0, bool editMode = false, QWidget *parent = 0);

protected slots:
	void onIntegrationModeUpdate(double value);
	void onIntegrationModeChange(int index);
	void setEditMode(bool editMode);
	void setEditable();

protected:
	PGTDetector *detector_;
	bool editMode_;
	QDoubleSpinBox *integrationTimeFbk_;
	QComboBox *integrationModeFbk_;
	QDoubleSpinBox *hvFbk_;
};

class MCPOldDetectorView : public MCPOldDetectorInfoView
{
	Q_OBJECT
public:
	MCPOldDetectorView(MCPDetector *detector, AMDetectorInfo *configDetector = 0, bool editMode = false, QWidget *parent = 0);

protected slots:
	void setEditMode(bool editMode);
	void setEditable();

protected:
	MCPDetector *detector_;
	bool editMode_;
	QDoubleSpinBox *hvFbk_;
};

class AMOldDetectorSetView : public AMDetectorInfoSetView
{
	Q_OBJECT
public:
	AMOldDetectorSetView(AMOldDetectorInfoSet *viewSet, AMOldDetectorInfoSet *configSet = 0, bool setup = true, QWidget *parent = 0);

public slots:
	void setEditMode(bool editMode);
	void setEditable();

protected:
	bool editMode_;

	virtual void runSetup();
	virtual QWidget* detailViewByType(AMDetectorInfo *detector, AMDetectorInfo *configDetector);
};


#endif // AMDETECTORVIEW_H
