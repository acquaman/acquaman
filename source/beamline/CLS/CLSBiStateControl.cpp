#include "CLSBiStateControl.h"

CLSBiStateControl::CLSBiStateControl(const QString &name, const QString &description, const QString &state, const QString &open, const QString &close, AMAbstractControlStatusChecker *statusChecker, QObject *parent)
	: AMControl(name, "n/a", parent, description)
{
}
