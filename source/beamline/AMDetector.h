#ifndef ACQMAN_AMDETECTOR_H
#define ACQMAN_AMDETECTOR_H

#include <QObject>
#include "AMControl.h"

class AMDetector : public QObject
{
Q_OBJECT
public:
	explicit AMDetector(QString name, QObject *parent = 0);

	~AMDetector(){;}

	QString name() const { return name_;}

//signals:

public slots:
	bool setName(const QString &name) { name_ = name; return true;}

protected:
	QString name_;
};

class AMSingleControlDetector : public AMDetector
{
Q_OBJECT
public:
	AMSingleControlDetector(QString name, AMControl *control, QObject *parent = 0);// : AMDetector(name, parent) {control_ = control;}
	~AMSingleControlDetector(){ control_ = NULL;}

protected:
	AMControl *control_;
};

#endif // AMDETECTOR_H
