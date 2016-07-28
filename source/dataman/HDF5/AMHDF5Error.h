#ifndef AMHDF5ERROR_H
#define AMHDF5ERROR_H

#include <QObject>

class AMHDF5Error : public QObject
{
	Q_OBJECT
public:
	explicit AMHDF5Error(QObject *parent = 0);

signals:

public slots:
};

#endif // AMHDF5ERROR_H