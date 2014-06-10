#ifndef AMSCANACTIONCONTROLLERSCANCONFIGURATIONCONVERTER_H
#define AMSCANACTIONCONTROLLERSCANCONFIGURATIONCONVERTER_H

#include <QObject>

class AMScanActionControllerScanAssembler;
class AMScanConfiguration;

class AMScanActionControllerScanConfigurationConverter : public QObject
{
Q_OBJECT
public:
 	virtual ~AMScanActionControllerScanConfigurationConverter();
	AMScanActionControllerScanConfigurationConverter(AMScanActionControllerScanAssembler *scanAssembler = 0, AMScanConfiguration *scanConfiguration = 0, QObject *parent = 0);

	bool convert();

public slots:
	void setScanAssembler(AMScanActionControllerScanAssembler *scanAssembler);
	void setScanConfiguration(AMScanConfiguration *scanConfiguration);

protected:
	virtual bool convertImplementation(AMScanActionControllerScanAssembler *scanAssembler, AMScanConfiguration *scanConfiguration) = 0;

private:
	AMScanActionControllerScanAssembler *scanAssembler_;
	AMScanConfiguration *scanConfiguration_;
};

#endif // AMSCANACTIONCONTROLLERSCANCONFIGURATIONCONVERTER_H
