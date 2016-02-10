#include "CLSJJSlit.h"
#include "beamline/CLS/CLSMAXvMotor.h"

CLSJJSlit::CLSJJSlit(const QString &name, const QString &positiveBladeName, const QString &negativeBladeName, QObject *parent) :
	AMSlit(name, parent)
{
	addBlade(new CLSMAXvMotor(QString("%1%2").arg(name).arg("Blade1"), baseName, QString("%1%2").arg(name).arg("Blade1"), true, 0.005, 2.0, this), AMSlit::OpensPositively);
	addBlade(new CLSMAXvMotor(QString("%1%2").arg(name).arg("Blade2"), baseName, QString("%1%2").arg(name).arg("Blade2"), true, 0.005, 2.0, this), AMSlit::OpensNegatively);
}

CLSJJSlit::~CLSJJSlit()
{

}
