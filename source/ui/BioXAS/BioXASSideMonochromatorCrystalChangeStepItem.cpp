#include "BioXASSideMonochromatorCrystalChangeStepItem.h"

BioXASSideMonochromatorCrystalChangeStepItem::BioXASSideMonochromatorCrystalChangeStepItem(QObject *parent) :
    QStandardItem(parent)
{
    icons_.insert("default", QIcon(":/applications-science.png"));
    icons_.insert("succeeded", QIcon(":/22x22/greenCheck.png"));
    icons_.insert("cancelled", QIcon(":/22x22/orangeX.png"));
    icons_.insert("failed", QIcon(":/22x22/redCrash.png"));
    icons_.insert("running", QIcon(":/22x22/dialog-question.png"));

    backgroundColors_.insert("default", this->background());
    backgroundColors_.insert("succeeded", QBrush(QColor(126, 255, 106)));
    backgroundColors_.insert("cancelled", QBrush(QColor(255, 176, 106)));
    backgroundColors_.insert("failed", QBrush(QColor(255, 104, 106)));
    backgroundColors_.insert("running", QBrush(QColor(100, 149, 237)));

    setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

BioXASSideMonochromatorCrystalChangeStepItem::~BioXASSideMonochromatorCrystalChangeStepItem()
{

}
