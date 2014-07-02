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


#include "AMScanActionTreeSupport.h"

#include "actions3/AMListAction3.h"

namespace AMScanActionTreeSupport{
	QList<AMAction3*> findActionsWhereNameContains(AMAction3 *rootAction, const QString &searchString){
		QList<AMAction3*> retVal;
		AMListAction3 *castToListAction = qobject_cast<AMListAction3*>(rootAction);
		if(castToListAction){
			for(int x = 0; x < castToListAction->subActionCount(); x++){
				AMListAction3 *subActionToListAction = qobject_cast<AMListAction3*>(castToListAction->subActionAt(x));
				if(subActionToListAction && subActionToListAction->info()->shortDescription().contains(searchString))
					retVal.append(subActionToListAction);
				retVal.append((findActionsNamed(castToListAction->subActionAt(x), searchString)));
			}
		}
		return retVal;
	}

	QList<AMAction3*> findActionsNamed(AMAction3 *rootAction, const QString &name){
		QList<AMAction3*> retVal;
		AMListAction3 *castToListAction = qobject_cast<AMListAction3*>(rootAction);
		if(castToListAction){
			for(int x = 0; x < castToListAction->subActionCount(); x++){
				AMListAction3 *subActionToListAction = qobject_cast<AMListAction3*>(castToListAction->subActionAt(x));
				if(subActionToListAction && subActionToListAction->info()->shortDescription() == name)
					retVal.append(subActionToListAction);
				retVal.append((findActionsNamed(castToListAction->subActionAt(x), name)));
			}
		}
		return retVal;
	}

	QList<AMAction3*> findActionsOfType(AMAction3 *rootAction, AMAction3 *typeAction){
		QList<AMAction3*> retVal;
		AMListAction3 *castToListAction = qobject_cast<AMListAction3*>(rootAction);
		if(castToListAction){
			for(int x = 0; x < castToListAction->subActionCount(); x++){
				if(typeAction->metaObject()->className() == castToListAction->subActionAt(x)->metaObject()->className())
					retVal.append(castToListAction->subActionAt(x));
				retVal.append((findActionsOfType(castToListAction->subActionAt(x), typeAction)));
			}
		}
		return retVal;
	}
}
