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


#ifndef AMIMPORTCONTROLLER_H
#define AMIMPORTCONTROLLER_H

#include <QObject>
#include "dataman/AMScan.h"

/// This abstract class defines the interface for an "importer" that does the specific heavy-lifting for importing a given file format.
class AMImporter {
public:
	virtual ~AMImporter() {}

	/// A human-readable description of what this importer is good for (ex: "CLS SGM Data Format, 2004 - 2010")
	virtual QString description() const = 0;

	/// The type (ie: class name) of the scan type returned by a succesful import (ex: "AMXASScan")
	virtual QString scanType() const = 0;

	/// Returns a file name filter appropriate for designating "importable files". (ex: "*.dat" or, for multiple allowed: "*.dat *.info *.xas"). This is used as a hint to try using this importer, but it doesn't restrict using it when the file name does not match.
	virtual QString fileNameFilter() const = 0;

	/// Attempt to import, and return a pointer to a new scan object, or 0 if the import fails.  It's the caller's responsibility to delete the scan when done with it.   \c fullPath should be an absolute path to the data file to import (or a path relative from the application's current working directory, but I wouldn't recommend that approach.)
	virtual AMScan* createScanAndImport(const QString& fullPath) = 0;
};

/// \todo Add plugin system to make it easier to add new AMImporter's.


/// This implementation of AMImporter is appropriate for importing the legacy (2004 - 2010) SGM file format, and returning an AMXASScan
class SGMLegacyImporter : public AMImporter {
public:
	/// A human-readable description of what this importer is good for (ex: "CLS SGM Data Format, 2004 - 2010")
	virtual QString description() const {
		return "CLS SGM data format, 2004 - 2010";
	}

	/// The type (ie: class name) of the scan type returned by a succesful import (ex: "AMXASScan")
	virtual QString scanType() const {
		return "AMXASScan";
	}

	/// Returns a file name filter appropriate for designating "importable files". (ex: "*.dat" or, for multiple allowed: "*.dat *.info *.xas"). This is used as a hint to try using this importer, but it doesn't restrict using it when the file name does not match.
	virtual QString fileNameFilter() const {
		return "*.dat";
	}

	/// Attempt to import, and return a pointer to a new scan object, or 0 if the import fails.  It's the caller's responsibility to delete the scan when done with it. \c fullPath should be an absolute path to the data file to import (or a path relative from the application's current working directory, but I wouldn't recommend that approach.)
	virtual AMScan* createScanAndImport(const QString& fullPath);
};


/// This implementation of AMImporter is appropriate for importing the ALS Beamline 8.0.1 (200? - 2010) XAS file format, and returning an AMXASScan
class ALSBL8XASImporter : public AMImporter {
public:
	/// A human-readable description of what this importer is good for
	virtual QString description() const {
		return "ALS Beamline 8.0.1 XAS data format, 200? - 2010";
	}

	/// The type (ie: class name) of the scan type returned by a succesful import (ex: "AMXASScan")
	virtual QString scanType() const {
		return "AMXASScan";
	}

	/// Returns a file name filter appropriate for designating "importable files". (ex: "*.dat" or, for multiple allowed: "*.dat *.info *.xas"). This is used as a hint to try using this importer, but it doesn't restrict using it when the file name does not match.
	virtual QString fileNameFilter() const {
		return "*.txt";
	}

	/// Attempt to import, and return a pointer to a new scan object, or 0 if the import fails.  It's the caller's responsibility to delete the scan when done with it.
	virtual AMScan* createScanAndImport(const QString& fullPath);
};


class QFileDialog;
class AMImportControllerWidget;

/// An instance of this class is used to supervise an import process, usually in response to a menu action.  The process is not modal (or blocking), but instead runs in the normal event loop.  When the process is finished, the controller deletes itself and all of its widgets, so you can simply call "new AMImportController()" and then forget about it.
class AMImportController : public QObject
{
Q_OBJECT
public:
	explicit AMImportController(QObject *parent = 0);
	virtual ~AMImportController();

signals:

public slots:

protected slots:
	/// Used to start the whole import process. (Called automatically from the constructor)
	void onStart();
	/// Called when the file dialog has finished (and files have been selected)
	void onFileDialogFinished(int result);
	/// Called when the 'next' button is clicked while reviewing
	void onNextButtonClicked();
	/// Called when the 'apply to all' button is clicked while reviewing
	void onApplyAllButtonClicked();
	/// called when the cancel button is clicked while reviewing
	void onCancelButtonClicked();

	/// handles setting up the review widget for the next file
	void setupNextFile();

	/// handles saving/importing the current file before moving on
	void finalizeImport();
	/// copies the raw data file into the correct location in the user's data folder, as well as any additionalFilePaths(). Updates the fileName() and additionalFilePaths() with the new locations. Called by finalizeImport(). Returns true on success.  The \c errorMessage will be filled with any error explanations if there is a problem.
	bool copyRawDataFiles(QString& errorMessage);

	/// called when the whole process is finished.
	void onFinished();
	/// while reviewing, the file type sets the method used to import. If it's changed, need to retry.
	void onFileTypeComboBoxChanged(int index);
	/// Called when the user changes the state of the Customize Name checkbox in the Widget
	void onCustomizeNameChanged(bool);
	/// Called when the user changes the state of the Customize Number checkbox in the Widget
	void onCustomizeNumberChanged(bool);
	/// Called when the user changes the state of the Customize DateTime checkbox in the Widget
	void onCustomizeDateTimeChanged(bool);

	/// Temporary solution only: Identifies what looks like a "spectra" file: an auxiliary file used by the CLS dacq library, which shouldn't be imported on its own.
	bool isAuxiliaryFile(const QString& fullFileName);


protected:
	/// The importers we have at our disposal:
	QList<AMImporter*> importers_;
	/// You must edit this function to add more importers. \todo Create plugin system
	void installImporters();

	QStringList filesToImport_;
	int currentFile_, numSuccess_;
	AMScan* currentScan_;
	bool applyToAll_;

	QFileDialog* fileDialog_;
	AMImportControllerWidget* importWidget_;



};

#endif // AMIMPORTCONTROLLER_H
