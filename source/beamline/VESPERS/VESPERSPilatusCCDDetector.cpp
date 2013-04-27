#include "VESPERSPilatusCCDDetector.h"

VESPERSPilatusCCDDetector::VESPERSPilatusCCDDetector(const QString &name, const QString &description, QObject *parent)
	: VESPERSCCDDetector(name, description, "PAD1607-B21-05:cam1", AMnDIndex(1000, 1000), parent)
{
}
