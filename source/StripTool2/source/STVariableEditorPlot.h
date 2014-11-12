#ifndef STVARIABLEEDITORPLOT_H
#define STVARIABLEEDITORPLOT_H

#include <QWidget>
#include <QCheckBox>
#include <QSpinBox>
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
    void onLineThicknessChanged();
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
    QSpinBox *lineThickness_;
    QCheckBox *showMarkers_;

    bool colorEdited_;
    bool lineThicknessEdited_;
    bool showMarkersEdited_;
};

#endif // STVARIABLEEDITORPLOT_H
