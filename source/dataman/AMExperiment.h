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


#ifndef AMEXPERIMENT_H
#define AMEXPERIMENT_H



#include "dataman/database/AMDbObject.h"
#include <QImage>

#include <QBuffer>
#include <QByteArray>
#include <QPixmap>

/// This class represents instances of objects that map to Experiments in the database of user information.
class AMExperiment : public AMDbObject
{
	Q_OBJECT
	Q_PROPERTY(QString notes READ notes WRITE setNotes)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Experiment")

public:
	/// Default constructor. In it we initialize the extra fields we want to store inside metaData_.
 	virtual ~AMExperiment();
	Q_INVOKABLE explicit AMExperiment(QObject *parent = 0);

	/// This constructor initializes an experiment with a given name.
	AMExperiment(const QString& experimentName, QObject* parent = 0);

	/// This constructor immediately loads a stored example from the database.
	AMExperiment(int databaseId, AMDatabase* database, QObject* parent = 0);

	/// Static function for managing experiments: deletes an experiment and all associated entries from the given \c database.  Returns true on success.
	/*! \todo move the general functionality in AMDbObject sytem */
	static bool deleteExperiment(int id, AMDatabase* database);

	/// Static function for managing experiments: find all the experiments that contain a given scan. Returns a list of experiment ids.
	static QList<int> experimentsContainingScan(int scanId, AMDatabase* database);

	/// Static function for managing experiments: adds a scan to an experiment
	static bool addScanToExperiment(int scanId, int experimentId, AMDatabase* database);

	/// Static function for managing experiments: remove scan from an experiment
	static bool removeScanFromExperiment(int scanId, int experimentId, AMDatabase* database);

	/// This returns a string of notes/comments about this experiment.
	QString notes() const {
		return notes_;
	}


signals:

public slots:

	/// Convenience function to set the notes/comments on this sample
	void setNotes(const QString& notes) {
		notes_ = notes;
		setModified(true);
	}



protected:
	/// User-logged notes about this experiment.
	QString notes_;

};

#endif // AMEXPERIMENT_H

