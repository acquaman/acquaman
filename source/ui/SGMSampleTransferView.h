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


#ifndef SGMSAMPLETRANSFERVIEW_H
#define SGMSAMPLETRANSFERVIEW_H

#include <QWidget>
#include <QGridLayout>
#include <QStackedLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <beamline/SGM/SGMBeamline.h>

class SGMSampleTransferPaneView;
class SGMSampleTransferProceduresView;

/* NTBA March 14, 2011 David Chevrier
   Need to upgrade all of these actions to new action style
   */

class SGMSampleTransferView : public QWidget
{
Q_OBJECT
public:
	explicit SGMSampleTransferView(QWidget *parent = 0);

signals:

public slots:

private slots:
	void drawMain();
	void drawIndex1();
	void drawIndex2();
	void drawIndex3();
	void drawIndex4();

protected:
	QList<QPushButton*> transferButtons_;
	QList<SGMSampleTransferPaneView*> transferPanes_;
	SGMSampleTransferProceduresView *transferBox_;
	QStackedLayout *mainLayout_;
};

class SGMSampleTransferProceduresView : public QGroupBox
{
Q_OBJECT
public:
	SGMSampleTransferProceduresView(const QString &title, QList<QPushButton*> procedureButtons, QWidget *parent = 0);

protected:
	QVBoxLayout *vl_;
	QList<QPushButton*> procedureButtons_;
	QGridLayout *mainLayout_;
};

class SGMSampleTransferPaneView : public QGroupBox
{
Q_OBJECT
public:
	explicit SGMSampleTransferPaneView(QList<AM1BeamlineActionItem*> items, const QString &title, QWidget *parent = 0);

signals:
	void completed();

public slots:
	virtual void startPane(){ initialize(); firstItem_->start();}

private slots:
	virtual void initialize();
	virtual void prepareCompletion();

protected:
	QVBoxLayout *vl_;
	QList<AMBeamlineActionItemView*> itemViews_;
	AM1BeamlineActionItem* firstItem_;
	QLabel *completeLabel_;
	QPushButton *completeButton_;
	QGridLayout *mainLayout_;
};

#endif // SGMSAMPLETRANSFERVIEW_H
