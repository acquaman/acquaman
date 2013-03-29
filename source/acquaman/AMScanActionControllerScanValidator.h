#ifndef AMSCANACTIONCONTROLLERSCANVALIDATOR_H
#define AMSCANACTIONCONTROLLERSCANVALIDATOR_H

#include <QObject>

#include "acquaman/AMScanActionTreeSupport.h"

class AMAction3;

class AMScanActionControllerScanValidator : public QObject
{
Q_OBJECT
public:
	AMScanActionControllerScanValidator(AMAction3 *scanActionTree, QObject *parent = 0);

public slots:
	bool validate();

protected:
	virtual bool validateImplementation(AMAction3 *scanActionTree) = 0;

private:
	AMAction3 *scanActionTree_;
};

#endif // AMSCANACTIONCONTROLLERSCANVALIDATOR_H
