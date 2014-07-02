#include "STPlotWidget.h"

STPlotWidget::STPlotWidget(QWidget *parent) : MPlotWidget(parent)
{    
    plotName_ = "Unnamed plot";
    plotNameVisible_ = false;

    // create plot and set up axes.
    MPlot *plot = new MPlot();
    plot->axisBottom()->setAxisName("Time");
    plot->axisLeft()->setAxisName("Storage ring current [mA]");

    setPlot(plot);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect( this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)) );
}

STPlotWidget::~STPlotWidget()
{

}

void STPlotWidget::setPlotName(const QString &newName)
{
    if (plotName_ != newName) {
        plotName_ = newName;
        plot_->axisTop()->setAxisName(plotName_);
        showPlotName(plotNameVisible_);

        emit plotNameChanged(plotName_);
    }
}

void STPlotWidget::showPlotName(bool show)
{
    plot_->axisTop()->showAxisName(show);
}

void STPlotWidget::showPlotEditor(bool show)
{
    if (show) {
        STPlotEditor *plotEditor = new STPlotEditor(this);
    }
}

void STPlotWidget::showAddVariableDialog()
{
    bool ok;
    QString name = QInputDialog::getText(this, "Add Variable", "PV Name :", QLineEdit::Normal, "", &ok);

    if (ok && !name.isEmpty())
        emit addVariable(name);
}

void STPlotWidget::onCustomContextMenuRequested(QPoint position)
{
    QMenu menu(this);

    QAction *add = menu.addAction("Add variable");
    QAction *edit = menu.addAction("Edit plot");

    QAction *selected = menu.exec(mapToGlobal(position));

    if (selected) {

        if (selected->text() == "Add variable") {
            showAddVariableDialog();

        } else if (selected->text() == "Edit plot") {
            showPlotEditor(true);

        }
    }
}

STPlotEditor* STPlotWidget::createPlotEditor()
{
    return new STPlotEditor();
}
