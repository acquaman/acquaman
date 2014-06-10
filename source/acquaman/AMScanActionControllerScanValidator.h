#ifndef AMSCANACTIONCONTROLLERSCANVALIDATOR_H
#define AMSCANACTIONCONTROLLERSCANVALIDATOR_H

#include <QObject>

#include "acquaman/AMScanActionTreeSupport.h"

class AMAction3;

class AMScanActionControllerScanValidator : public QObject
{
Q_OBJECT
public:
 	virtual ~AMScanActionControllerScanValidator();
	AMScanActionControllerScanValidator(AMAction3 *scanActionTree = 0, QObject *parent = 0);

public slots:
	bool validate();

	void setScanActionTree(AMAction3 *scanActionTree);

protected:
	virtual bool validateImplementation(AMAction3 *scanActionTree) = 0;

private:
	AMAction3 *scanActionTree_;
};

#endif // AMSCANACTIONCONTROLLERSCANVALIDATOR_H
