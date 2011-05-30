#ifndef GENERALUTILITIES_H
#define GENERALUTILITIES_H

#include <QObject>

class GeneralUtilities : public QObject
{
	Q_OBJECT
public:

	/// Takes a emission line name and turns the greek letter into a roman equivalent and returns the result (ie: alpha -> a).
	static QString removeGreek(QString name);

signals:

public slots:

private:
	// This class is not meant to be instantiated.
	explicit GeneralUtilities(QObject *parent = 0);
};

#endif // GENERALUTILITIES_H
