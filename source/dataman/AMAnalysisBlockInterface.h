#ifndef AMANALYSISBLOCKINTERFACE_H
#define AMANALYSISBLOCKINTERFACE_H

class AMAnalysisBlockInterface
{
public:
	virtual ~AMAnalysisBlockInterface() {}
	virtual bool accepts(const QString &format) = 0;
};

QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(AMAnalysisBlockInterface,
		    "AMAnalysisBlockInterface/1.0");
QT_END_NAMESPACE

#endif // AMFILELOADERINTERFACE_H
