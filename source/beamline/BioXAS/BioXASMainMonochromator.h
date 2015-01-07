#ifndef BIOXASMAINMONOCHROMATOR_H
#define BIOXASMAINMONOCHROMATOR_H

#include <QObject>

class BioXASMainMonochromator : public QObject
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASMainMonochromator(QObject *parent = 0);
    /// Destructor.
    virtual ~BioXASMainMonochromator();

signals:

public slots:

};

#endif // BIOXASMAINMONOCHROMATOR_H
