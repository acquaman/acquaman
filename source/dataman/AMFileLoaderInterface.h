#ifndef AMFILELOADERINTERFACE_H
#define AMFILELOADERINTERFACE_H

//#include "dataman/AMScan.h"

class AMScan;
class QString;

class AMFileLoaderInterface
{
public:
	virtual ~AMFileLoaderInterface() {}
	virtual bool accepts(AMScan *scan) = 0;

	virtual bool load(AMScan *scan, const QString &userDataFolder) = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(AMFileLoaderInterface,
			"AMFileLoaderInterface/1.0");
QT_END_NAMESPACE

#endif // AMFILELOADERINTERFACE_H
