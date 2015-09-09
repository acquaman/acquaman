#ifndef SGMMONOCHROMATORINFO_H
#define SGMMONOCHROMATORINFO_H

#include <QObject>
#include <QHash>
class SGMMonochromatorInfo : public QObject
{
    Q_OBJECT
public:
    explicit SGMMonochromatorInfo(QObject *parent = 0);

	QHash<QString, double> buildValues() const;

	void testValues() const;
};

#endif // SGMMONOCHROMATORINFO_H
