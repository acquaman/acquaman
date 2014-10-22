#include "BeamPositionMonitorView.h"
//#include "sidebar.h"

#include <QVBoxLayout>

#include <QDebug>

#include "BeamlineShutters.h"
#include "BeamlineShutterView.h"

int action_height = 70;


/*
SideBar::SideBar(QWidget *parent)
    : QWidget(parent), _pressedAction(NULL), _checkedAction(NULL)
{
    setFixedWidth(70);
}

SideBar::~SideBar()
{

}

*/


void BeamPositionMonitorView::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

    QFont fontText(p.font());
   // fontText.setFamily("Helvetica");
    fontText.setFamily("Sans Serif");
    fontText.setBold(true);
    fontText.setPixelSize(16);
    p.setFont(fontText);

    QImage texture("/home/david/Desktop/147860-Sidebar/Sidebar/texture3.png");
    p.fillRect(event->rect(), QBrush(texture));
    p.setPen(Qt::black);
    p.drawLine(event->rect().topRight(), event->rect().bottomRight());

    int actions_height = _actions.size()*action_height;

    int action_y = event->rect().height()/2-actions_height/2;
    foreach(QAction *action, _actions)
    {
        QRect actionRect(0, action_y, event->rect().width(), action_height);

        if(action->isChecked())
        {
            p.setOpacity(0.5);
            p.fillRect(actionRect, QColor(19, 19, 19));
            p.setPen(QColor(9, 9, 9));
            p.drawLine(actionRect.topLeft(), actionRect.topRight());
            p.setOpacity(1);

        }

        if(action == _actions.last())
        {
            p.setPen(QColor(15, 15, 15));
            p.drawLine(QPoint(0, actionRect.bottomLeft().y()-1), QPoint(actionRect.width(), actionRect.bottomRight().y()-1));
            p.setPen(QColor(55, 55, 55));
            p.drawLine(actionRect.bottomLeft(), actionRect.bottomRight());

        }

        if(!action->isChecked())
        {
            p.setPen(QColor(15, 15, 15));
            p.drawLine(actionRect.topLeft(), actionRect.topRight());
            p.setPen(QColor(55, 55, 55));
            p.drawLine(QPoint(0, actionRect.topLeft().y()+1), QPoint(actionRect.width(), actionRect.topRight().y()+1));
        }



        QRect actionIconRect(0, action_y, event->rect().width(), action_height-20);
        QIcon  actionIcon(action->icon());
        actionIcon.paint(&p, actionIconRect);


        p.setPen(QColor(217, 217, 217));
        if(action->isChecked())
            p.setPen(QColor(255, 239, 3));

        QRect actionTextRect(0, action_y+actionRect.height()-20, event->rect().width(), 25);
        p.drawText(actionTextRect, Qt::AlignCenter, action->text());

        action_y += actionRect.height();
    }

}

QSize BeamPositionMonitorView::minimumSizeHint() const
{
    return QSize(90, _actions.size()*action_height);
}

void BeamPositionMonitorView::addAction(QAction *action)
{
    _actions.push_back(action);
    action->setCheckable(true);
    update();
    return;
}

QAction *BeamPositionMonitorView::addAction(const QString &text, const QIcon &icon)
{
    QAction *action = new QAction(icon, text, this);
    action->setCheckable(true);
    _actions.push_back(action);
    update();
    return action;
}

void BeamPositionMonitorView::mousePressEvent(QMouseEvent *event)
{
    _pressedAction = actionAt(event->pos());
    if(_pressedAction == NULL || _pressedAction == _checkedAction)
        return;
    update();
}

void BeamPositionMonitorView::mouseReleaseEvent(QMouseEvent *event)
{
    QAction* tempAction = actionAt(event->pos());
    if(_pressedAction != tempAction || tempAction == NULL)
    {
        _pressedAction = NULL;
        return;
    }
    if(_checkedAction != NULL)
        _checkedAction->setChecked(false);
    _checkedAction = _pressedAction;
    if(_checkedAction != NULL)
        _checkedAction->setChecked(true);

    if( tempAction->text() == "Shutters" )
    {
        if ( displayShutters == false){
            wid->show();
            displayShutters = true;
            _checkedAction->setChecked(true);
        }
        else if ( displayShutters == true){
           wid->hide();
            displayShutters = false;
            _checkedAction->setChecked(false);
        }

    }
    else if( tempAction->text() == "Beam Position" )
    {

        if ( displayBPM == false){
            this->viewWidget_->show();
            displayBPM = true;
            _checkedAction->setChecked(true);
        }
        else if ( displayBPM == true){
            this->viewWidget_->hide();
            displayBPM = false;
            _checkedAction->setChecked(false);
        }

     }
    else if( tempAction->text() == "Exit" )
    {
        ::exit(0);

    }

    update();
    _pressedAction = NULL;
    return;
}

QAction* BeamPositionMonitorView::actionAt(const QPoint &at)
{
    int actions_height = _actions.size()*action_height;

    int action_y = rect().height()/2-actions_height/2;
    foreach(QAction *action, _actions)
    {
        QRect actionRect(0, action_y, rect().width(), action_height);
        if(actionRect.contains(at))
            return action;
        action_y += actionRect.height();
    }
    return NULL;
}




////////////////////////////////////////////

BeamPositionMonitorView::BeamPositionMonitorView(QWidget *parent)
    : QWidget(parent){

    displayBPM = false;
    displayShutters = false;


    ///For testing purposes. Moved BeamlineShutter objects from FCApp.cpp to this location

    BeamlineShutters *cmcfidShutters = new BeamlineShutters("CMCF-ID",
                                                             "PSH1408-I00-01:state",
                                                             "PSH1408-I00-02:state",
                                                             "SSH1408-I00-01:state",
                                                             this);

    BeamlineShutters *bmitShutters = new BeamlineShutters("BMIT",
                                                             "PSH1405-B10-01:state",
                                                             "PSH1405-B10-02:state",
                                                             "SSH1405-B10-01:state",
                                                             this);

    BeamlineShutters *sylmandShutters = new BeamlineShutters("SyLMAND",
                                                             "PSH1406-B20-01:state",
                                                             "PSH1406-B20-02:state",
                                                             "SSH1406-B20-01:state",
                                                             this);

    BeamlineShutters *hxmaShutters = new BeamlineShutters("HXMA",
                                                             "PSH1406-I00-01:state",
                                                             "PSH1406-I00-02:state",
                                                             "SSH1406-I00-01:state",
                                                             this);

    BeamlineShutters *sxrmbShutters = new BeamlineShutters("SXRMB",
                                                             "PSH1406-B20-01:state",
                                                             "PSH1406-B20-02:state",
                                                             "SSH1406-B20-01:state",
                                                             this);


    BeamlineShutters *vespersShutters = new BeamlineShutters("VESPERS",
                                                             "PSH1408-B20-01:state",
                                                             "PSH1408-B20-02:state",
                                                             "SSH1408-B20-01:state",
                                                             this);

    BeamlineShutters *cmcfbmShutters = new BeamlineShutters("CMCF-BM",
                                                             "PSH1408-B10-01:state",
                                                             "PSH1408-B10-02:state",
                                                             "SSH1408-B10-01:state",
                                                             this);

    BeamlineShutters *ideasShutters = new BeamlineShutters("IDEAS",
                                                             "PSH1409-B20-01:state",
                                                             "PSH1409-B20-02:state",
                                                             "SSH1409-B20-01:state",
                                                             this);

    BeamlineShutters *smShutters = new BeamlineShutters("SXRMB",
                                                             "PSH1410-I00-01:state",
                                                             "PSH1410-I00-02:state",
                                                             "SSH1410-I00-01:state",
                                                             this);

    BeamlineShutters *sgmpgmShutters = new BeamlineShutters("PGM/SGM",
                                                             "PSH1406-B20-01:state",
                                                             "PSH1406-B20-02:state",
                                                             "SSH1406-B20-01:state",
                                                             this);


    bmitView_ = new BeamlineShutterView(bmitShutters);
    sylmandView_ = new BeamlineShutterView(sylmandShutters);
    hxmaView_ = new BeamlineShutterView(hxmaShutters);
    sxrmbView_ = new BeamlineShutterView(sxrmbShutters);
    vespersView_ = new BeamlineShutterView(vespersShutters);
    cmcfidView_ = new BeamlineShutterView(cmcfidShutters);
    cmcfbmView_ = new BeamlineShutterView(cmcfbmShutters);
    ideasView_ = new BeamlineShutterView(ideasShutters);
    smView_ = new BeamlineShutterView(smShutters);
    sgmpgmView_ = new BeamlineShutterView(sgmpgmShutters);

    QVBoxLayout *shutterLayout = new QVBoxLayout;
    shutterLayout->setSpacing(0);
    shutterLayout->addWidget(cmcfidView_);
    shutterLayout->addWidget(bmitView_);
    shutterLayout->addWidget(sylmandView_);
    shutterLayout->addWidget(hxmaView_);
    shutterLayout->addWidget(sxrmbView_);
    shutterLayout->addWidget(vespersView_);
    shutterLayout->addWidget(cmcfbmView_);
    shutterLayout->addWidget(ideasView_);
    shutterLayout->addWidget(smView_);
    shutterLayout->addWidget(sgmpgmView_);



    QPalette Background(palette());
    Background.setColor(QPalette::Background, Qt::black);

      cmcfidView_->setAutoFillBackground(true);
      cmcfidView_->setPalette(Background);
      bmitView_->setAutoFillBackground(true);
      bmitView_->setPalette(Background);
      sylmandView_->setAutoFillBackground(true);
      sylmandView_->setPalette(Background);
      hxmaView_->setAutoFillBackground(true);
      hxmaView_->setPalette(Background);
      sxrmbView_->setAutoFillBackground(true);
      sxrmbView_->setPalette(Background);
      vespersView_->setAutoFillBackground(true);
      vespersView_->setPalette(Background);
      cmcfidView_->setAutoFillBackground(true);
      cmcfidView_->setPalette(Background);
      cmcfbmView_->setAutoFillBackground(true);
      cmcfbmView_->setPalette(Background);
      ideasView_->setAutoFillBackground(true);
      ideasView_->setPalette(Background);
      smView_->setAutoFillBackground(true);
      smView_->setPalette(Background);
      sgmpgmView_->setAutoFillBackground(true);
      sgmpgmView_->setPalette(Background);



     wid = new QWidget;
     wid->setLayout(shutterLayout);
     wid->hide();

    /////////////////////////////////////////////


    bpmXY_ = new BeamPositionMonitor("BPM:XRMS", "BPM:YRMS", this);
    bpmDataSource_ = new MPlotVectorSeriesData;

    setupPlot();
    this->addAction("Shutters", QIcon("/home/david/Desktop/147860-Sidebar/Sidebar/icons/light61.png"));
    this->addAction("Beam Position", QIcon("/home/david/Desktop/147860-Sidebar/Sidebar/icons/monitoring1.png"));
    this->addAction("Exit", QIcon("/home/david/Desktop/147860-Sidebar/Sidebar/icons/exit13.png"));
    this->show();


    /*
    QVBoxLayout *plotLayout = new QVBoxLayout;
    plotLayout->addWidget(viewWidget_);

    */

    //setLayout(plotLayout);





    connect(bpmXY_, SIGNAL(specifiedValuesCollected()), this, SLOT(updateVectorsFromModel()));
 //connect(bpmXY_, SIGNAL(newValuesFromPV()), this, SLOT(updateVectorsFromModel()));

}
void BeamPositionMonitorView::updateVectorsFromModel(){

    tempX = bpmXY_->xList();
    tempY = bpmXY_->yList();

    xVector_ = tempX.toVector();
    yVector_ = tempY.toVector();

    bpmDataSource_->setValues(xVector_, yVector_);


}


void BeamPositionMonitorView::setupPlot(){

    //Create plot window
    viewWidget_ = new MPlotWidget;

    //Create the plot and setup axis
    plot_ = new MPlot;
    plot_->axisBottom()->setAxisNameFont(QFont("Helvetica", 12));
    plot_->axisBottom()->setTickLabelFont(QFont("Helvetica", 12));
    plot_->axisBottom()->setAxisName("X  (um)");

    plot_->axisLeft()->setAxisNameFont(QFont("Helvetica", 12));
    plot_->axisLeft()->setTickLabelFont(QFont("Helvetica", 12));
    plot_->axisLeft()->setAxisName("Y (um)");

    plot_->setMarginLeft(10);
    plot_->setMarginBottom(15);
    plot_->setMarginRight(2);
    plot_->setMarginTop(2);

    //Creates a series to be viewed using the XY scatter data source object
    //from MPlotVectorSeriesData.
    scatter_ = new MPlotSeriesBasic;
    scatter_->setModel(bpmDataSource_, true);
    scatter_->setMarker(MPlotMarkerShape::PointCircle, 14);
    scatter_->setDescription(" ");

    //Add series to plot
    plot_->addItem(scatter_);

    //Put the plot into a plot window, the figures used here will be subject to change as I sort out how I want it to look
    viewWidget_->setPlot(plot_);
    viewWidget_->setMinimumSize(500, 450);


    plot_->axisScaleBottom()->setAutoScaleEnabled();
    plot_->axisScaleLeft()->setAutoScaleEnabled();

   // plot_->axisScaleBottom()->setDataRangeAndDisableAutoScaling(MPlotAxisRange(-0.2, 0.1));
   // plot_->axisScaleLeft()->setDataRangeAndDisableAutoScaling(MPlotAxisRange(-0.1, 0.2));
    plot_->axisBottom()->setTicks(4, MPlotAxis::Middle);
    plot_->axisTop()->setTicks(4, MPlotAxis::Middle);
    plot_->axisLeft()->setTicks(4, MPlotAxis::Middle);
    plot_->axisRight()->setTicks(4, MPlotAxis::Middle);



      viewWidget_->hide();

}

