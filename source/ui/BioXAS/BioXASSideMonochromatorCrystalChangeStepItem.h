#ifndef BIOXASSIDEMONOCHROMATORCRYSTALCHANGESTEPITEM_H
#define BIOXASSIDEMONOCHROMATORCRYSTALCHANGESTEPITEM_H

#include <QStandardItem>

class BioXASSideMonochromatorCrystalChangeStepItem : public QStandardItem
{
    Q_OBJECT
public:
    explicit BioXASSideMonochromatorCrystalChangeStepItem(QObject *parent = 0);
    virtual BioXASSideMonochromatorCrystalChangeStepItem();

signals:

public slots:


protected:
    QMap<QString, QIcon> icons_;
    QMap<QString, QBrush> backgroundColors_;

};

#endif // BIOXASSIDEMONOCHROMATORCRYSTALCHANGESTEPITEM_H
