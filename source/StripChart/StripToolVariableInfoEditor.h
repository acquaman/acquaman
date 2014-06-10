#ifndef STRIPTOOLVARIABLEINFOEDITOR_H
#define STRIPTOOLVARIABLEINFOEDITOR_H

#include <QObject>

#include "StripChart/StripToolVariableInfo.h"
#include "StripChart/EditPVDialog.h"

class StripToolVariableInfoEditor : public QObject
{
    Q_OBJECT

public:
    explicit StripToolVariableInfoEditor(QObject *parent = 0);
    friend class StripTool;

protected:
    void editInfo(StripToolVariableInfo* toEdit);

private:
    StripToolVariableInfo* info() const;
    EditPVDialog* dialog();
    void setDialogFromInfo();
    void executeDialog();

private:
    StripToolVariableInfo* info_;
    EditPVDialog* dialog_;

};

#endif // STRIPTOOLVARIABLEINFOEDITOR_H
