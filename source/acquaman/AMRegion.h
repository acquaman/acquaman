#ifndef ACQMAN_AMREGION_H
#define ACQMAN_AMREGION_H

#include <QObject>
#include <QAbstractTableModel>
#include <QList>
#include "beamline/AMControl.h"

/*
  FIX ME UP! I want to be able to have elastic regions ... start value is last end plus this delta.
class ContinuousRegion
{
public:
    ContinuousRegion(){};
    double startVal_;
    QList<double> values_;
};
*/

class AMRegion: public QObject
{
Q_OBJECT
Q_PROPERTY(double start READ start WRITE setStart)
Q_PROPERTY(double delta READ delta WRITE setDelta)
Q_PROPERTY(double end READ end WRITE setEnd)

public:
AMRegion(QObject *parent = 0);
    virtual double start() const { return start_;}
    virtual double delta() const { return delta_;}
    virtual double end() const { return end_;}
    virtual AMControl* control() const { return ctrl_;}

public slots:
    virtual bool setStart(double start) { start_ = start; return TRUE;}
    virtual bool setDelta(double delta) {
        if(delta == 0)
            return false;
        delta_ = delta;
        return true;
    }
    virtual bool setEnd(double end) { end_ = end; return TRUE;}
    virtual bool setControl(AMControl* ctrl) { ctrl_ = ctrl; return true;}

protected:
    double start_;
    double delta_;
    double end_;
    AMControl *ctrl_;
};

class AMXASRegion : public AMRegion
{
Q_OBJECT

public:
AMXASRegion(AMControl* beamlineEnergy, QObject *parent = 0) : AMRegion(parent) {ctrl_ = beamlineEnergy;}

public slots:
    bool setStart(double start) {
        if(ctrl_->valueOutOfRange(start))
            return FALSE;
        start_ = start;
        return TRUE;
    }
    bool setEnd(double end) {
        if(ctrl_->valueOutOfRange(end))
            return FALSE;
        end_ = end;
        return TRUE;
    }
    bool setControl(AMControl *ctrl){Q_UNUSED(ctrl); return false;}
};

class AMXASRegionModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	AMXASRegionModel(QList<AMXASRegion*> *regions, QObject *parent = 0) : QAbstractTableModel(parent) {
		regions_ = regions;
	}

	int rowCount(const QModelIndex & /*parent*/) const { return regions_->count(); }
	int columnCount(const QModelIndex & /*parent*/) const { return 3; }

	QVariant data(const QModelIndex &index, int role) const {

		// Invalid index:
		if(!index.isValid())
			return QVariant();

		// We only answer to Qt::DisplayRole right now
		if(role != Qt::DisplayRole)
			return QVariant();

		// Out of range: (Just checking for too big.  isValid() checked for < 0)
		if(index.row() >= regions_->count())
			return QVariant();
		// Return Start:
		if(index.column() == 0)
			return regions_->at(index.row())->start() ;
		// Return Delta:
		if(index.column() == 1)
			return regions_->at(index.row())->delta() ;
		if(index.column() == 2)
			return regions_->at(index.row())->end() ;

		// Anything else:
		return QVariant();
	}

	QVariant headerData(int section, Qt::Orientation orientation, int role) const {

		if (role != Qt::DisplayRole) return QVariant();

		// Vertical headers:
		if(orientation == Qt::Vertical) {
			return section;
		}

		// Horizontal Headers: (Column labels)
		else {
			if(section == 0)
				return "Start";
			if(section == 1)
				return "Delta";
			if(section == 2)
				return "End";
		}
		return QVariant();
	}

	bool setData(const QModelIndex &index, const QVariant &value, int role) {

		if (index.isValid()  && index.row() < regions_->count() && role == Qt::EditRole) {

			bool conversionOK;
			double dval = value.toDouble(&conversionOK);
			if(!conversionOK)
				return false;

			// Setting a start value?
			if(index.column() == 0) {
				regions_->at(index.row())->setStart(dval);
				emit dataChanged(index, index);
				return true;
			}
			// Setting a delta value?
			if(index.column() == 1) {
				regions_->at(index.row())->setDelta(dval);
				emit dataChanged(index, index);
				return true;
			}
			// Setting an end value?
			if(index.column() == 1) {
				regions_->at(index.row())->setEnd(dval);
				emit dataChanged(index, index);
				return true;
			}
		}
		return false;	// no value set
	}

	// This allows editing of values within range (for ex: in a QTableView)
	Qt::ItemFlags flags(const QModelIndex &index) const {

		Qt::ItemFlags flags;
		if (index.isValid() && index.row() < regions_->count() && index.column()<3)
			flags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
		return flags;
	}

private:
//	void setupModelData(const QStringList &lines, TreeItem *parent);

	QList<AMXASRegion*> *regions_;
};

#endif // ACQMAN_AMREGION_H
