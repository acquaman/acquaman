#ifndef AMSAMPLEPERIODICTABLEDIALOG_H
#define AMSAMPLEPERIODICTABLEDIALOG_H

#include "AMPeriodicTableDialog.h"
#include <QList>
class AMElement;

class AMSamplePeriodicTableDialog : public AMPeriodicTableDialog
{
    Q_OBJECT
public:
    AMSamplePeriodicTableDialog(QList<const AMElement*> elements, QWidget* parent = 0);

    static const AMElement* getElement(QList<const AMElement*> elements, QWidget* parent = 0);

protected:
    QList<const AMElement*> elements_;
};

#endif // AMSAMPLEPERIODICTABLEDIALOG_H
