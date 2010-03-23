#include "acqLibHelper.h"

/*int
deleteEventPv( acqEvent_t *ev, int rowNum)
{
        int idx;
        int maxPv;

        if( ev == NULL)
                return -1;
        maxPv = (ev->numPvList >= NUM_EVENT_PV)?ev->numPvList:NUM_EVENT_PV;
        if( rowNum >= maxPv)
                return -1;

        if( ev->pvList[rowNum].pvName)
                free(ev->pvList[rowNum].pvName) ;
        if( ev->pvList[rowNum].outputFormat )
                free(ev->pvList[rowNum].outputFormat);
        ev->pvList[rowNum].pvName = NULL;
        ev->pvList[rowNum].outputFormat = NULL;
        for( idx = rowNum; idx < maxPv-1; idx++)
        {
//                ev->pvList[idx] = ev->pvList[idx+1];
                ev->pvList[idx].pvName = ev->pvList[idx+1].pvName;
                ev->pvList[idx].noRecord = ev->pvList[idx+1].noRecord;
                ev->pvList[idx].ready = ev->pvList[idx+1].ready;
                ev->pvList[idx].isSpectrum = ev->pvList[idx+1].isSpectrum;
                ev->pvList[idx].displayRow = ev->pvList[idx+1].displayRow;
                ev->pvList[idx].outputFormat = ev->pvList[idx+1].outputFormat;
                ev->pvList[idx].event = ev;
        }
        // following lines are in case we're copying garbage from a higher location
        ev->pvList[idx].pvName = NULL;
        ev->pvList[idx].outputFormat = NULL;
        ev->numPvList--;
        return 0;
}
*/

int
addIndexEventPv( acqEvent_t *ev, int index, char *name, int norecord, char *fmt, acqPvReady_t ready, int isSpectrum)
{
    int idx;

    if( name == NULL || ! *name || (index < 0) )   /* don't add null PV's */
            return -1;

    idx = ev->numPvList;
    if( index >= NUM_EVENT_PV)
    {
            int nbytes;
            if( ev->numPvList < NUM_EVENT_PV)
                    ev->numPvList = NUM_EVENT_PV;
            nbytes = (1+ev->numPvList)* sizeof (*ev->pvList);
//            DEBUG printf("adding event %d total bytes %d (%d*%d)\n", idx, nbytes, ev->numPvList+1, sizeof(*ev->pvList));
            printf("adding event %d total bytes %d (%d*%d)\n", idx, nbytes, ev->numPvList+1, sizeof(*ev->pvList));
            ev->pvList = realloc(ev->pvList, nbytes);
    }
    ev->numPvList++;

    for(idx = ev->numPvList; idx >= 0; idx--)
    {
        if(idx == index)
        {
            ev->pvList[idx].pvName = strdup( name);
            ev->pvList[idx].noRecord = norecord;
            ev->pvList[idx].ready = ready;
            ev->pvList[idx].isSpectrum = isSpectrum;
            ev->pvList[idx].displayRow = -1;
            if( fmt && *fmt)
                ev->pvList[idx].outputFormat = strdup( fmt);
            else
                ev->pvList[idx].outputFormat = NULL;
            ev->pvList[idx].event = ev;
            break;
        }
        else
        {
            printf("\nadding at %d\n", idx); 
            ev->pvList[idx].pvName = ev->pvList[idx-1].pvName;
            ev->pvList[idx].noRecord = ev->pvList[idx-1].noRecord;
            ev->pvList[idx].ready = ev->pvList[idx-1].ready;
            ev->pvList[idx].isSpectrum = ev->pvList[idx-1].isSpectrum;
            ev->pvList[idx].displayRow = ev->pvList[idx-1].displayRow;
            ev->pvList[idx].outputFormat = ev->pvList[idx-1].outputFormat;
            ev->pvList[idx].event = ev;
//            ev->pvList[idx] = ev->pvList[idx-1];
            printf("\nafter adding at %d\n", idx);
        }
    }
    return idx;
}

/*
int addEventPv( acqEvent_t *ev, char *name, int norecord, char *fmt, acqPvReady_t ready, int isSpectrum)
{ 
        int idx;
   
        if( name == NULL || ! *name )
                return -1;
 	for(idx = 0; idx < NUM_EVENT_PV; idx++)
                if( ev->pvList[idx].pvName == NULL)
                        break;
        
        if( idx >= NUM_EVENT_PV)
        {
                int nbytes;
                if( ev->numPvList < NUM_EVENT_PV)
                        ev->numPvList = NUM_EVENT_PV;
                idx = ev->numPvList;
                nbytes = (1+ev->numPvList)* sizeof (*ev->pvList);
         	DEBUG printf("adding event %d total bytes %d (%d*%d)\n", idx, nbytes, ev->numPvList+1, sizeof(*ev->pvList));
                ev->pvList = realloc(ev->pvList, nbytes);
        }
        ev->numPvList++;
        ev->pvList[idx].pvName = strdup( name);
        ev->pvList[idx].noRecord = norecord;
        ev->pvList[idx].ready = ready;
        ev->pvList[idx].isSpectrum = isSpectrum;
        ev->pvList[idx].displayRow = -1;
        if( fmt && *fmt)
                ev->pvList[idx].outputFormat = strdup( fmt);
        else
            	ev->pvList[idx].outputFormat = NULL;

	ev->pvList[idx].event = ev;
        return idx;
}
*/
