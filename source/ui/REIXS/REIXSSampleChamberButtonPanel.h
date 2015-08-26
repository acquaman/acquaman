/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef REIXSSAMPLECHAMBERBUTTONPANEL_H
#define REIXSSAMPLECHAMBERBUTTONPANEL_H

#include <QWidget>

class QString;
class QIcon;
class QToolButton;

class AMControlMoveButton;
class AMCompositeControlDirectionMoveButton;


class REIXSSampleChamberButtonPanel : public QWidget
{
    Q_OBJECT

public:
    explicit REIXSSampleChamberButtonPanel(QWidget *parent = 0);
    virtual ~REIXSSampleChamberButtonPanel();

public slots:
	void onJoystickButtonChanged(int buttonId, bool isDown);

protected slots:
	void onStopButtonClicked();

protected:
	void setupUi();
	void initializeUiComponents();

	QToolButton *createQToolButton(QString text, QIcon icon);
	AMControlMoveButton *createAMControlMoveButton(QString text, QIcon icon);
	AMCompositeControlDirectionMoveButton *createAMCompositeControlDirectionMoveButton(QString text, QIcon icon);

private:
	AMControlMoveButton *sampleXup_;
	AMControlMoveButton *sampleXdown_;
	AMControlMoveButton *sampleYdown_;
	AMControlMoveButton *sampleYup_;
	AMControlMoveButton *sampleZup_;
	AMControlMoveButton *sampleZdown_;
	AMControlMoveButton *sampleCW_;
	AMControlMoveButton *sampleCCW_;
	AMCompositeControlDirectionMoveButton *sampleLeft_;
	AMCompositeControlDirectionMoveButton *sampleRight_;
	AMCompositeControlDirectionMoveButton *sampleIn_;
	AMCompositeControlDirectionMoveButton *sampleOut_;

	QToolButton *stopAll_;
	QToolButton *stopAll2_;
	QToolButton *stopAll3_;
};

#endif // REIXSSAMPLECHAMBERBUTTONPANEL_H
