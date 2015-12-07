#ifndef BIOXASZEBRA_H
#define BIOXASZEBRA_H

#include <QObject>

#include "beamline/AMPVControl.h"

/// This is a class for controlling the zebra triggering box.
class BioXASZebra : public QObject
{
	Q_OBJECT
public:
	/// Constructor.
	explicit BioXASZebra(const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASZebra();

signals:

public slots:

protected:
	/// Helper command function.  If given a source name, it returns the number.  Number is required for all source settings.
	int commandFromName(const QString &commandName) const;
};

#endif // BIOXASZEBRA_H
