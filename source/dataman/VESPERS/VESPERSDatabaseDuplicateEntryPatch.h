#ifndef VESPERSDATABASEDUPLICATEENTRYPATCH_H
#define VESPERSDATABASEDUPLICATEENTRYPATCH_H

#include <QObject>

/// Class that takes a database, finds all duplicate entries and removes them.  Fixes all associated tables and references as well.
class VESPERSDatabaseDuplicateEntryPatch : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	explicit VESPERSDatabaseDuplicateEntryPatch(QObject *parent = 0);

signals:

public slots:
	/// Starts the patch process.
	bool start();

};

#endif // VESPERSDATABASEDUPLICATEENTRYPATCH_H
