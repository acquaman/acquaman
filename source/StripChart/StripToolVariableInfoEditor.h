#ifndef STRIPTOOLVARIABLEINFOEDITOR_H
#define STRIPTOOLVARIABLEINFOEDITOR_H

#include <QObject>

#include "StripChart/StripToolBasicVariableInfo.h"
#include "StripChart/EditPVDialog.h"

class StripToolVariableInfoEditor : public QObject
{
    Q_OBJECT
public:
    explicit StripToolVariableInfoEditor(StripToolBasicVariableInfo *info, QObject *parent = 0);

private:
    StripToolBasicVariableInfo* info_;
    EditPVDialog* dialog_;

};

#endif // STRIPTOOLVARIABLEINFOEDITOR_H
