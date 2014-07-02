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


//#ifndef STRIPTOOLABSTRACTVARIABLEINFO_H
//#define STRIPTOOLABSTRACTVARIABLEINFO_H

//#include <QObject>

//class StripToolAbstractVariableInfo : public QObject
//{
//    Q_OBJECT

//    friend class StripToolAbstractVariable;
//    friend class StripToolModel;
//    friend class StripToolVariableInfoEditor;
//    friend class StripToolVariableInfoImporter;
//    friend class StripToolVariableInfoExporter;

//public:
//    explicit StripToolAbstractVariableInfo(QObject *parent = 0);

//signals:
//    void infoChanged();

//public:
//    virtual bool hasName() const;
//    virtual bool hasCreationDateTime() const;
//    virtual bool hasDescription() const;
//    virtual bool hasUnits() const;
//    virtual bool hasGranularity() const;
//    virtual bool hasColor() const;
//    virtual bool hasSelectionState() const;
//    virtual bool hasCheckState() const;
//    virtual bool hasTimeUnits() const;

//    QString name() const;
//    QString description() const;

//public slots:
//    void setName(const QString &sourceName);
//    void setDescription(const QString &newDescription);

//private:
//    QString name_;
//    QString description_;

//    bool hasName_;
//    bool hasCreationDateTime_;
//    bool hasDescription_;
//    bool hasUnits_;
//    bool hasGranularity_;
//    bool hasColor_;
//    bool hasSelectionState_;
//    bool hasCheckState_;
//    bool hasTimeUnits_;

//};

//#endif // STRIPTOOLABSTRACTVARIABLEINFO_H
