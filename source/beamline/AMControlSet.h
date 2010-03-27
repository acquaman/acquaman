#ifndef AMCONTROLSET_H
#define AMCONTROLSET_H

#include <QObject>
#include <QList>
#include "AMControl.h"

class AMControlSet : public QObject
{
Q_OBJECT
public:
    explicit AMControlSet(QObject *parent = 0);

    QString name() const { return name_;}
    //const QList<const AMControl*> controls() const { return ctrls_;}
    QList<AMControl*> controls() { return ctrls_;}

signals:

public slots:
    void setName(const QString &name) { name_ = name;}
    bool addControl(AMControl* ctrl) {
        if(ctrls_.contains(ctrl))
            return false;
        ctrls_.append(ctrl);
        return true;
    }
    bool removeControl(AMControl* ctrl) { return ctrls_.removeOne(ctrl);}

protected:
    QString name_;
    QList<AMControl*> ctrls_;

};

#endif // AMCONTROLSET_H
