/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMVerticalStackWidget.h"

#include "ui/AMHeaderButton.h"


AMVerticalStackWidget::AMVerticalStackWidget(QWidget *parent, bool usingGrouping) :
		QFrame(parent)
{
	usingGrouping_ = usingGrouping;
	currentGroup_ = 0;
	vl_ = new QVBoxLayout();
	vl_->setSpacing(0);
	vl_->setContentsMargins(0,0,0,0);
	vl_->setAlignment(Qt::AlignTop);

	vlSide_ = new QVBoxLayout();
	vlSide_->setSpacing(0);
	vlSide_->setContentsMargins(0,0,0,0);
	vlSide_->setAlignment(Qt::AlignTop);

	hl_ = new QHBoxLayout();
	hl_->setSpacing(0);
	hl_->setContentsMargins(0,0,0,0);

	hl_->addLayout(vl_);
	hl_->addLayout(vlSide_);

	//setLayout(vl_);
	setLayout(hl_);

	setFrameShape(QFrame::StyledPanel);
	setFrameShadow(QFrame::Sunken);
	setStyleSheet("AMVerticalStackWidget {background:white; }");
}


// Return the index of a widget in the stack. (-1 if not found)
int AMVerticalStackWidget::indexOf(QWidget* widget) const {
	for(int i=0; i<count(); i++)
		if( model_.data(model_.index(i,0), AM::PointerRole).value<QWidget*>() == widget)
			return i;
	return -1;
}
// Return the widget at a specific index, or 0 for invalid indexes
QWidget* AMVerticalStackWidget::widget(int index) const {
	if(index<0 || index>=count())
		return 0;
	return model_.data(model_.index(index,0), AM::PointerRole).value<QWidget*>();
}

#include "beamline/AMBeamlineActionItem.h"
// Insert a widget at a specific index in the stack. Inserting at \c index = -1 is equivalent to appending to the end. The AMVerticalStackWidget takes ownership of the widget.
void AMVerticalStackWidget::insertItem(int index, const QString& titleText, QWidget* widget, bool collapsable) {
	if(index < 0 || index > count())
		index = count();

	if(widget->windowTitle() != titleText)
		widget->setWindowTitle(titleText);

	QStandardItem* item = new QStandardItem(titleText);
	item->setData(qVariantFromValue(widget), AM::PointerRole);
	item->setData(false, Qt::CheckStateRole);
	item->setFlags( collapsable ? (Qt::ItemIsEnabled | Qt::ItemIsUserCheckable) : Qt::ItemIsEnabled);

	AMHeaderButton* header = new AMHeaderButton();
	header->setText(titleText);
	header->setArrowType(Qt::DownArrow);
	item->setData(qVariantFromValue(header), AM::WidgetRole);
	connect(header, SIGNAL(clicked()), this, SLOT(onHeaderButtonClicked()));

	widget->installEventFilter(this);

	QSizePolicy sp = widget->sizePolicy();
	sp.setVerticalPolicy(QSizePolicy::Preferred);
	widget->setSizePolicy(sp);

	vl_->insertWidget(2*index, header);
	vl_->insertWidget(2*index+1, widget);

	if(usingGrouping_){
		connect(header, SIGNAL(heightChanged(int)), this, SLOT(onWidgetHeightChanged(int)));
		AMBeamlineActionItemView *aiv = qobject_cast<AMBeamlineActionItemView*>(widget);
		if(aiv)
			connect(aiv, SIGNAL(heightChanged(int)), this, SLOT(onWidgetHeightChanged(int)));

		if(!currentGroup_){
			qDebug() << "Trying to draw group widget";
			AMRunGroupWidget *rgWidget = new AMRunGroupWidget("", this);
			currentGroup_ = rgWidget;
			rgWidget->setFixedWidth(24);
			//rgWidget->setFrameStyle(QFrame::StyledPanel);
			vlSide_->insertWidget(index, rgWidget);
		}
	}

	model_.insertRow(index, item);
}

bool AMVerticalStackWidget::swapItem(int indexOfFirst){
	if(indexOfFirst < 0 || indexOfFirst > count()-2)
		return false;

	QWidget *headerOfFirst = vl_->takeAt(2*indexOfFirst)->widget();
	QWidget *widgetOfFirst = vl_->takeAt(2*indexOfFirst)->widget();

	vl_->insertWidget(2*(indexOfFirst+1), headerOfFirst);
	vl_->insertWidget(2*(indexOfFirst+1)+1, widgetOfFirst);

	//QStandardItem *itemOfFirst = model_.takeItem(indexOfFirst);
	QStandardItem *itemOfFirst = model_.takeRow(indexOfFirst).at(0);
	model_.insertRow(indexOfFirst+1, itemOfFirst);
	return true;
}


// Remove a widget and return it.
QWidget* AMVerticalStackWidget::takeItem(int index) {
	if(index<0 || index>=count())
		return 0;

	QStandardItem* item = model_.takeRow(index).at(0);
	QWidget* w = item->data(AM::PointerRole).value<QWidget*>();
	AMHeaderButton* b = item->data(AM::WidgetRole).value<AMHeaderButton*>();

	vl_->takeAt(2*index);
	vl_->takeAt(2*index);

	delete b;
	delete item;

	w->removeEventFilter(this);
	return w;
}

// Is this item collapsable?
bool AMVerticalStackWidget::itemCollapsable(int index) const {
	if(index < 0 || index >= count())
		return false;

	QStandardItem* item = model_.item(index,0);
	return (item->flags() & Qt::ItemIsUserCheckable);
}
// Is this item collapsed?
bool AMVerticalStackWidget::itemIsCollapsed(int index) const {
	if(index < 0 || index >= count())
		return false;

	return model_.data(model_.index(index,0), Qt::CheckStateRole).toBool();
}

// Set the heading text for a widget
void AMVerticalStackWidget::setItemText(int index, const QString& text) {
	if(index < 0 || index >= count())
		return;

	QStandardItem* item = model_.item(index, 0);
	item->setData(text, Qt::DisplayRole);
	AMHeaderButton* tb = item->data(AM::WidgetRole).value<AMHeaderButton*>();
	if(tb)
		tb->setText(text);
}


// Expand a given widget
void AMVerticalStackWidget::expandItem(int index) {
	if(index < 0 || index >= count())
		return;

	QWidget* w = model_.data(model_.index(index, 0), AM::PointerRole).value<QWidget*>();
	if(w)
		w->show();
	AMHeaderButton* h = model_.data(model_.index(index,0), AM::WidgetRole).value<AMHeaderButton*>();
	if(h){
		h->setArrowType(Qt::DownArrow);
		h->forceHeightChanged();
	}
	model_.setData(model_.index(index,0), false, Qt::CheckStateRole);
}

// Collapse a given widget
void AMVerticalStackWidget::collapseItem(int index) {
	if(index < 0 || index >= count())
		return;

	QWidget* w = model_.data(model_.index(index, 0), AM::PointerRole).value<QWidget*>();
	if(w){
		w->hide();
		qDebug() << "Is visible " << w->isVisible();
	}
	AMHeaderButton* h = model_.data(model_.index(index,0), AM::WidgetRole).value<AMHeaderButton*>();
	if(h){
		h->setArrowType(Qt::RightArrow);
		h->forceHeightChanged();
	}
	model_.setData(model_.index(index,0), true, Qt::CheckStateRole);
}


// \todo Optimize this so we don't have to linear search through the whole model to find out which header was clicked
void AMVerticalStackWidget::onHeaderButtonClicked() {
	QObject* s = sender();
	for(int i=0; i<count(); i++) {
		if(model_.data(model_.index(i,0), AM::WidgetRole).value<AMHeaderButton*>() == s) {
			if(itemCollapsable(i))
				itemIsCollapsed(i) ? expandItem(i) : collapseItem(i);
			return;
		}
	}
}

void AMVerticalStackWidget::onWidgetHeightChanged(int newHeight){
	if(usingGrouping_ && (groupings_.count() > 0) ){
		int groupingsCount = 0;
		for(int x = 0; x < groupings_.count(); x++)
			groupingsCount += groupings_.at(x).first;
		if(groupingsCount != vl_->count()/2){
			qDebug() << "Mismatch " << groupingsCount << " versus " << vl_->count()/2;
			return;
		}
		int groupHeight = 0;
		int groupingsCounter = 0;
		int toThisPoint = 0;
		qDebug() << "Looking at a height change " << groupingsCount << vl_->count();
		for(int x = 0; x < vl_->count(); x++){
			if(vl_->itemAt(x)->widget()->isVisible())
				groupHeight += vl_->itemAt(x)->widget()->height();
			else
				qDebug() << "That sucker is closed, don't count it";
			if( x == (toThisPoint + groupings_.at(groupingsCounter).first)*2-1 ){
				qDebug() << "Set to group height " << groupHeight << " on x " << x;
				vlSide_->itemAt(groupingsCounter)->widget()->setFixedHeight(groupHeight);
				AMRunGroupWidget *runGroup = qobject_cast<AMRunGroupWidget*>(vlSide_->itemAt(groupingsCounter)->widget());
				if(runGroup){
					runGroup->setDisplayText(groupings_.at(groupingsCounter).second);
					runGroup->update();
				}
				toThisPoint += groupings_.at(groupingsCounter).first;
				groupingsCounter++;
				groupHeight = 0;
			}
		}
		qDebug() << "DONE PAINTING SIDE BARS";
		update();
		emit doneRunGroups();
		return;
	}
	qDebug() << "Not using grouping or no groupings";
}

QSize AMVerticalStackWidget::sizeHint() const {
	QSize rv = QFrame::sizeHint();

	int width = rv.width();
	for(int i=0; i<count(); i++) {
		width = qMax(width, model_.data(model_.index(i,0), AM::PointerRole).value<QWidget*>()->sizeHint().width());
	}

	rv.setWidth(width+2);
	return rv;
}

void AMVerticalStackWidget::startRunning(){
	qDebug() << "Trying to draw group widget because RUNNING STARTED";
	AMRunGroupWidget *rgWidget = new AMRunGroupWidget("", this);
	//currentGroup_ = rgWidget;
	rgWidget->setFixedWidth(24);
	//rgWidget->setFrameStyle(QFrame::StyledPanel);
	vlSide_->insertWidget(vlSide_->count()-1, rgWidget);
}

void AMVerticalStackWidget::endRunning(){
	currentGroup_ = 0;
	QWidget * removed = vlSide_->itemAt(vlSide_->count()-1)->widget();
	vlSide_->removeWidget(removed);
	delete removed;
}

void AMVerticalStackWidget::setGroupings(QList<QPair<int, QString> > groupings){
	groupings_ = groupings;
	qDebug() << "Just set groupings";
}

void AMVerticalStackWidget::forceGroupingsCheck(){
	onWidgetHeightChanged(-1);
}

void AMVerticalStackWidget::forceUpdate(){
	qDebug() << "LOWEST LEVEL FORCED UPDATE";
	for(int x = 0; x < vl_->count(); x++)
		vl_->itemAt(x)->widget()->update();
	for(int x = 0; x < vlSide_->count(); x++){
		vlSide_->itemAt(x)->widget()->repaint();
		vlSide_->itemAt(x)->widget()->updateGeometry();
	}
	this->repaint();
	this->updateGeometry();
}


// Capture window title change events from our widgets and change our header titles accordingly
bool AMVerticalStackWidget::eventFilter(QObject * source, QEvent *event) {

	if(event->type() != QEvent::WindowTitleChange)
		return QFrame::eventFilter(source, event);

	int widgetIndex = this->indexOf(qobject_cast<QWidget*>(source));
	if(widgetIndex < 0)
		return QFrame::eventFilter(source, event);

	setItemText(widgetIndex, widget(widgetIndex)->windowTitle());

	return QFrame::eventFilter(source, event);
}


AMRunGroupWidget::AMRunGroupWidget(const QString &displayText, QWidget *parent) :
		QLabel(displayText, parent)
{
	displayText_ = displayText;
}

void AMRunGroupWidget::setDisplayText(const QString &displayText){
	displayText_ = displayText;
}

#include <QPainter>
void AMRunGroupWidget::paintEvent(QPaintEvent *event){
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	qreal w = width();
	qreal h = height();

	painter.setBrush(QBrush(Qt::black));
	painter.translate(0, 0);
	painter.drawLine(QPointF(0, 0), QPointF(w, 0));
	painter.drawLine(QPointF(w, 0), QPointF(w, h));
	painter.drawLine(QPointF(w, h), QPointF(0, h));
	painter.drawLine(QPointF(0, h), QPointF(0, 0));

	QFontMetrics metric(this->font());
	painter.setPen(Qt::black);
	QString displayElided = metric.elidedText(displayText_, Qt::ElideRight, h-0.1*h);
	painter.save();
	painter.translate(w, 0);
	painter.rotate(90.0);
	painter.drawText(QRectF(0.05*h, 0, metric.width(displayElided), w), Qt::AlignCenter, displayElided);
	painter.restore();
}

void AMRunGroupWidget::resizeEvent(QResizeEvent *event){
	QLabel::resizeEvent(event);
	qDebug() << "Resize event";
	update();
}
