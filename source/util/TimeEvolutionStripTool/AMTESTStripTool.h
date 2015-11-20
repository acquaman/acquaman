#ifndef AMTESTSTRIPTOOL_H
#define AMTESTSTRIPTOOL_H

#include <QObject>

#include "util/TimeEvolutionStripTool/AMTESTServerConfiguration.h"
#include "util/TimeEvolutionStripTool/AMTESTServerConnection.h"

/// This is the master time evolution strip tool model.
class AMTESTStripTool : public QObject
{
	Q_OBJECT

public:
	/// Singleton accessor.
	static AMTESTStripTool *stripTool();
	/// Releases the singleton and does necessary cleanup.
	static void releaseStripTool();

	/// Returns the list of server connections.
	QList<AMTESTServerConnection *> serverConnections() const { return serverConnections_; }
	/// Returns the server connection at the provided \param index.  Returns 0 if \param index is invalid.
	AMTESTServerConnection *serverConnectionAt(int index) const;
	/// Returns the server connnection from the provided \param name.  Returns 0 if \param name doesn't exist.
	AMTESTServerConnection *serverConnectionByName(const QString &name) const;

protected:
	/// Protected constructor for singleton.
	explicit AMTESTStripTool(QObject *parent = 0);

	/// Setup the server connections.
	void setupServerConnections();

	/// The singleton member.
	static AMTESTStripTool *stripTool_;

	/// A list of server connections.
	QList<AMTESTServerConnection *> serverConnections_;
};

#endif // AMTESTSTRIPTOOL_H
