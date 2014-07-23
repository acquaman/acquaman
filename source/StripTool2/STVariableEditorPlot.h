#ifndef STVARIABLEEDITORPLOT_H
#define STVARIABLEEDITORPLOT_H

#include <QWidget>
#include <QCheckBox>
#include <QLayout>
#include <QLabel>

#include "STEditor.h"
#include "STVariableEditor.h"
#include "STVariable.h"
#include "STColorButton.h"

class STVariableEditorPlot : public STVariableEditor
{
    Q_OBJECT

public:
    explicit STVariableEditorPlot(STVariable *toEdit, QWidget *parent = 0);
    virtual ~STVariableEditorPlot();

signals:

public slots:
    virtual void applyChanges();

protected slots:
    void onColorButtonClicked();
    void onShowMarkersChanged();

protected:
    virtual void getVariableInfo();
    virtual void clearVariableInfo();
    virtual void connectVariable() {return;}
    virtual void disconnectVariable() {return;}
    virtual void connectUI();
    virtual void disconnectUI();

protected:
    STColorButton *colorButton_;
    QCheckBox *showMarkers_;

    bool colorEdited_;
    bool showMarkersEdited_;
};

#endif // STVARIABLEEDITORPLOT_H
