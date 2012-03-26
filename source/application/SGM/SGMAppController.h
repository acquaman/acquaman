/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef SGMAPPCONTROLLER_H
#define SGMAPPCONTROLLER_H

#include "application/AMAppController.h"

class SGMSampleTransferView;
class AMSampleManagementWidget;
class CLSSIS3820ScalerView;
class AMDetectorView;
class AMScanConfigurationViewHolder;
class SGMXASScanConfigurationView;
class SGMFastScanConfigurationView;
class SGMSidebar;
class SGMSettingsMasterView;
class AMGithubManager;

class SGMAppController : public AMAppController {
	Q_OBJECT

public:
	/// This constructor should be empty. Call SGMAppController::startup() to create all of the application windows, widgets, and data objects that are needed on program startup.
	explicit SGMAppController(QObject* parent = 0);

	/// Destructor
	virtual ~SGMAppController() {}

	/// create and setup all of the application windows, widgets, communication connections, and data objects that are needed on program startup. Returns true on success.  If reimplementing, must call the base-class startup() as the first thing it does.
	virtual bool startup();

	/// destroy all of the windows, widgets, and data objects created by applicationStartup(). Only call this if startup() has ran successfully.  If reimplementing, must call the base-class shutdown() as the last thing it does.
	virtual void shutdown();

public slots:
	virtual bool startupCreateDatabases();
	virtual bool startupDatabaseUpgrades();

	/// Re-implemented from AMAppController to create and register the SGM database with the database system.
	virtual bool startupRegisterDatabases();
	virtual bool startupPopulateNewDatabase();
	virtual bool startupLoadFromExistingDatabase();

protected slots:
	/// This slot catches changes in the current widget of the AMMainWindow. \c pane is the new current widget.  Re-implement to catch any widget-specific responses that you need here.
	/*! \note This only applies to panes that are currently docked within the main window.  If a pane has been undocked, no notification will be received when it becomes raised or activated by the user.
	  */
	virtual void onCurrentPaneChanged(QWidget* pane);

	void onSGMBeamlineConnected();
	void onSGMScalerConnected(bool connected);
	void onSGMAmptekSDD1Connected(bool connected);
	void onSGMAmptekSDD2Connected(bool connected);

	/// \todo comment this, David...
	void onCurrentScanControllerCreated();
	void onCurrentScanControllerDestroyed();
	void onCurrentScanControllerStarted();

	void onActionSGMSettings();

protected:
	bool startupSGMInstallActions();
	bool setupSGMConfigurationFiles();
	bool setupSGMPlugins();
	bool setupSGMExporterOptions();
	bool setupSGMPeriodicTable();

	bool setupSGMViews();

	/// Upgrades an AMDbObject class originally called \c originalClassName to \c newClassName. Use this function carefully, incorrect or incomplete parameters can lead to corrupted databases.
	/*! This takes care of the problem where a class used in the database needs to be renamed. There are several caveats on this one.
	  The function is only for classes of medium complexity. Other AMDbObjects can store this class directly or lists of this class (for example, a class that inherits AMDetectorInfo is okay, but this can't handle something like AMDetectorInfoSet).
	  This function will ensure that the AMDbObjectTypes_table is properply updated, as well as changing the name of the class table and its AMDbObjectType column.
	  The \c parentTablesToColumnNames will map a table name to a column name and look in that table under that column to change instances of the "from" table name to the "to" table name.
	  The \c indexTablesToIndexSide will map an index table to a side (either the id1/table1 side or the id2/table2 side BUT THE id1/table1 SIDE IS NOT YET TESTED AND MAY NOT WORK) and change instances of the "from" table name to the "to" table name.
	  For example, if you have an index table you can speficy the index table name and the column to check in. This will run through all of the row of that index table and update instances of the old class name to the new class name.
	  */
	bool dbObjectClassBecomes(const QString &originalClassName, const QString &newClassName, QMap<QString, QString> parentTablesToColumnNames, QMap<QString, int> indexTablesToIndexSide);

	/// Merges two AMDbObject classes by taking the \c mergeFromClassName, converting them into the \c mergeToClassName and removing all of the database entities related to \c mergeFromClassName. Use this function carefully, incorrect or incomplete parameters can lead to corrupted databases. Not fully tested for the parentTables portion.
	/*! This function should be used if you have two IDENTICAL classes (all the same AMDbObjectProperties/database columns) and you want to remove the "from" class and keep the "to" class by converting the former to the latter.
	  The function is only for classes of medium complexity. Other AMDbObjects can store this class directly or lists of this class (for example, a class that inherits AMDetectorInfo is okay, but this can't handle something like AMDetectorInfoSet).
	  This function will ensure that the AMDbObjectTypes_table and its related tables are properply updated.
	  The \c parentTablesToColumnNames will map a table name to a column name and look in that table under that column to change instances of the "from" table name to the "to" table name.
	  The \c indexTablesToIndexSide will map an index table to a side (either the id1/table1 side or the id2/table2 side BUT THE id1/table1 SIDE IS NOT YET TESTED AND MAY NOT WORK) and update to new ids and change instances of the "from" table name to the "to" table name.
	  */
	bool dbObjectClassMerge(const QString &mergeToClassName, const QString &mergeFromClassName, QMap<QString, QString> parentTablesToColumnNames, QMap<QString, int> indexTablesToIndexSide);

protected:

	SGMSampleTransferView *sampleTransferView_;
	AMSampleManagementWidget *samplePositionView_;
	CLSSIS3820ScalerView *sgmScalerView_;
	AMDetectorView *amptekSDD1View_;
	AMDetectorView *amptekSDD2View_;
	SGMXASScanConfigurationView *xasScanConfigurationView_;
	SGMFastScanConfigurationView *fastScanConfigurationView_;
	AMScanConfigurationViewHolder *xasScanConfigurationHolder_;
	AMScanConfigurationViewHolder *fastScanConfigurationHolder_;
	SGMSidebar *sgmSidebar_;

	/// Persistent view for SGMSettings
	SGMSettingsMasterView *sgmSettingsMasterView_;
};

#endif // SGMAPPCONTROLLER_H
