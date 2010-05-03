/**
 ** $Header: acqLoad.c 1.3.1.3 2009/03/04 15:13:46CST Glen Wright (wrightg) Exp  $
 ** Copyright Canadian Light Source, Inc. All rights reserved.
 **
 ** support loading and saving of configuration files.
 */

#include "acquisitionLib.internal.h"
#include <stdio.h>

typedef enum { IN_UNKNOWN, IN_SCAN, IN_EVENT, IN_ACTION, IN_MOTOR, IN_FILE, IN_HANDLER } inState_t;

static void output_scan( FILE *fp, acqScan_t *sc);
static void output_event( FILE *fp, acqEvent_t *ev);
static void output_motor( FILE *fp, acqMotor_t *m);
static void input_configuration(FILE *fp , acqMaster_t *master);
static int gobble( char *input, char *pattern, void *addr, int *offset);

static int debugFlag;
#define DEBUG if(debugFlag)

void
acq_file_load(const char *fname , acqMaster_t *master)
{
	FILE *fp;
	acqEvent_t *ev, *next_ev;
	acqScan_t *sc, *next_sc;
	acqMotor_t *m, *next_m;

	for( ev = first_acqEvent(master); ev ; ev = next_ev )
	{
		next_ev = next_acqEvent(ev);
		destroy_acqEvent(ev , master);
	}
	for( sc = first_acqScan(master) ; sc ; sc  = next_sc)
	{
		next_sc = next_acqScan(sc);
		destroy_acqScan(sc , master);
	}
	for( m = first_acqMotor(master) ; m ; m = next_m)
	{
		next_m = next_acqMotor( m);
		destroy_acqMotor( m , master);
	}

	master->acqStatusMessage(master, "Loading config file %s", fname);
	fp = fopen(fname, "r");
	if( fp == NULL)
	{
		master->acqStatusMessage(master, "Unable to load file %s", fname);
		return;
	}
	input_configuration(fp , master);

	if( master->fileVersion < ACQ_VERSION(2,1,0) )
	{
		printf("Warning: configuration version older than 2.1.0 (%d). Please update!\n", master->fileVersion);
		for( sc = first_acqScan(master) ; sc ; sc = next_sc)
		{
			next_sc = next_acqScan(sc);
		}
	}
	return;
}

void
acq_file_save_as(const char *fname , acqMaster_t *master)
{
	acqEvent_t *ev;
	acqScan_t *sc;
	acqMotor_t *m;
	FILE *fp;

	master->acqStatusMessage(master, "Saving to file %s", fname);
	fp = fopen(fname, "w");
	if( fp == NULL)
	{
		master->acqStatusMessage(master, "Unable to write to %s", fname);
		return;
	}
	fprintf(fp, "# Version %d\n", ACQ_VERSION_CURRENT);
	for( sc = first_acqScan(master); sc ; sc = next_acqScan(sc) )
		output_scan(fp, sc);

	for( ev = first_acqEvent(master) ; ev ; ev = next_acqEvent(ev) )
		output_event(fp, ev);

	for( m = first_acqMotor(master) ; m ; m = next_acqMotor(m) )
		output_motor(fp, m);

	fclose(fp);
	return;
}

static void
output_scan_action_list( FILE *fp, int type_idx, acqAction_t *head)
{
	acqAction_t *curp;

	for( curp=head; curp; curp=(curp->next==head?NULL:curp->next) )
	{
		DEBUGM(head->master,1) printf("output_scan_action_list: %s %s\n", tgTypeName(type_idx), getActionName( curp->type) );
		fprintf(fp, "# Action %s %s ", tgTypeName(type_idx), getActionName( curp->type) );
		switch( curp->type)
		{
		case AA_DELAY_TIME:
			DEBUGM(head->master,1) printf("  delay %g\n", curp->au.dt.delay);
			fprintf( fp, "%g\n", curp->au.dt.delay);
			break;
		case AA_SET_PV:
			fprintf( fp, "\"%s\" \"%s\"\n", curp->au.spv.name, curp->au.spv.value);
			break;
		case AA_WAIT_PV:
			fprintf( fp, "\"%s\" \"%s\"\n", curp->au.wpv.name, curp->au.wpv.value);
			break;
		case AA_WAIT_MOTOR:
			fprintf( fp, "\"%s\"\n", curp->au.wm.name);
			break;
		case AA_CALL_EVENT:
			fprintf( fp, "\"%s\" %d\n", curp->au.ce.name, curp->au.ce.autoWait);
			break;
		case AA_CALL_SCAN:
			fprintf( fp, "\"%s\" %d\n", curp->au.cs.name, curp->au.cs.autoWait);
			break;
		case AA_WAIT_EVENT:
			fprintf( fp, "\"%s\"\n", curp->au.wm.name);
			break;
		case AA_WAIT_SCAN:
			fprintf( fp, "\"%s\"\n", curp->au.wm.name);
			break;
		case AA_SET_CONTROL:
			break;
		case AA_NEXT_OUTPUT:
			break;
		}
	}
}

static void
output_scan_control_value(FILE *fp,  char *prefix, int valid, double val, char *macro)
{
	if( valid && macro && *macro)
		fprintf(fp, "%s:\"%s\" ", prefix, macro);
	else
		fprintf(fp, "%s:%g ", prefix, val);
}

static void
output_scan( FILE *fp, acqScan_t *sc)
{
	int i;
	if( sc == NULL || sc->scanName == NULL)
		return;
	fprintf(fp, "# Scan \"%s\" %d onStart:%d\n", sc->scanName, sc->scanNum, sc->useStart);
	for( i=0; i < sc->numControlPV; i++)
	{
		acqControl_t *ctlp;
		int active;
		ctlp = &sc->acqControlList[i];
		active = (ctlp->haveStartVal?4:0)|(ctlp->haveDeltaVal?2:0)|(ctlp->haveFinalVal?1:0);
		fprintf(fp, "# Control \"%s\" ", ctlp->controlPV?ctlp->controlPV:"");
		output_scan_control_value(fp, "start", ctlp->haveStartVal, ctlp->startVal, ctlp->startMacro);
		output_scan_control_value(fp, "delta", ctlp->haveDeltaVal, ctlp->deltaVal, ctlp->deltaMacro);
		output_scan_control_value(fp, "final", ctlp->haveFinalVal, ctlp->finalVal, ctlp->finalMacro);
		fprintf(fp, "active:%d\n", active);
	}

	for( i=0; i < NUM_SCAN_TRIGGERS; i++)
		output_scan_action_list( fp, i, sc->actions[i]);

}

static void
output_event( FILE *fp, acqEvent_t *ev)
{
	int i;

	if( ev == NULL || ev->eventName == NULL)
		return;
	fprintf(fp, "# Event \"%s\" %d\n", ev->eventName, ev->eventNum);
	fprintf(fp, "# datastream columns: eventID, absolute/relative time stamps\n");
	fprintf(fp, "# eventID:%d AbsTime:%d Rel0Time:%d relPTime:%d\n",
		ev->putEventId, ev->putAbsTime, ev->putRel0Time, ev->putRelPTime);
	fprintf(fp, "# commentPrefix: %d\n", ev->putCommentPrefix);
	for( i=0; i < ev->numPvList; i++)
	{
		if( ev->pvList[i].pvName == NULL)
			continue;
		fprintf(fp, "# PV %d: \"%s\" disable:%d", i, ev->pvList[i].pvName, ev->pvList[i].noRecord);
		if( ev->pvList[i].outputFormat)
			fprintf(fp, " format:\"%s\"", ev->pvList[i].outputFormat);
		fprintf(fp, " spectrum:%d", ev->pvList[i].isSpectrum);
		fprintf(fp, " ready:%d", ev->pvList[i].ready);
		fprintf(fp, "\n");
	}
	fprintf(fp, "#\n");
}

static void
output_motor( FILE *fp, acqMotor_t *m)
{
	acqOneMotor_t *om;

	fprintf(fp, "# MotorGroup \"%s\"\n# Motors: ", m->name);
	for( om=m->list; om ; om=next_acqOneMotor(om) )
	{
		fprintf(fp, " \"%s\"", om->name);
	}
	fprintf(fp, "\n");
}

static int reuse_line;
static char dataline[500];
static char *
input_line( FILE *fp)
{
	char *s;

	if( reuse_line)
	{
		reuse_line = 0;
		return dataline;
	}
	if( fgets( dataline, sizeof dataline, fp) == NULL)
		return NULL;
	/*
	 * trim the newline
	 */
	s = strchr(dataline, '\n');
	if( s)
		*s = '\0';
	return dataline;
}

static void
input_configuration(FILE *fp , acqMaster_t *master)
{
	char *il;	/* input line */
	inState_t inputState;	/* track the section of input being scanned */
	char namebuf[100];	/* general buffer used by sscanf */
	int id;			/* scan/event ID */
	int nscan;		/* number of successful pattern matches from sscanf */

	acqControl_t ctls;
	acqEvent_t *ev = NULL;
	acqScan_t *sc = NULL;
	acqMotor_t *m = NULL;

	reuse_line = 0;
	inputState = IN_UNKNOWN;
	debugFlag = master->acqDebug;

	for(;;)
	{
		il = input_line(fp);
		if( il == NULL)
		{
			fclose(fp);
			break;
		}

		if( strcmp( il, "#") == 0)	/* exact match to 'comment only' */
			continue;

		switch( inputState)
		{
			case IN_UNKNOWN:
			{
			int onStart;

			if( sscanf( il, "# Version %d", &id) > 0)
			{
				master->fileVersion = id;
				continue;
			}
			if( sscanf( il, "# Scan \"%[^\"]\" %d onStart:%d", namebuf, &id, &onStart) == 3)
			{
				inputState = IN_SCAN;
				sc = new_acqScan(namebuf , master);
				sc->scanNum = id;
				sc->useStart = onStart;
				continue;
			}
			if( sscanf( il, "# Event \"%[^\"]\" %d", namebuf, &id) == 2)
			{
				inputState = IN_EVENT;
				ev = new_acqEvent( namebuf , master);
				ev->eventNum = id;
				continue;
			}

			if( sscanf( il, "# MotorGroup \"%[^\"]\"", namebuf) == 1)
			{
				inputState = IN_MOTOR;
				m = new_acqMotor( namebuf , master);
				continue;
			}

			if( sscanf( il, "# File: \"%[^\"]\"", namebuf) == 1)		/* deprecated, need more general output handler configuration */
			{
				inputState = IN_FILE;
				continue;
			}

			if( sscanf( il, "# Handler: \"%[^\"]\"", namebuf) == 1)		/* property list for a named handler */
			{
				inputState = IN_HANDLER;
				continue;
			}

			master->acqStatusMessage(master, "Unknown input '%s'\n", il);
			continue;
			}

			case IN_SCAN:
			{
			char typebuf[100], valuebuf[100];
			char startbuf[100], deltabuf[100], finalbuf[100];
			double delay;
			int active;
			int offset = 0;	/* used to index the scanned string */

			sscanf( il, "# Control \"%n", &offset);
			if( offset > 0)
			{
				int offset2;
				memset( &ctls, 0, sizeof ctls);
				DEBUGM(master,1) printf("Control: offset: %d remaining: (%s)\n", offset, il+offset);
				if( il[offset] != '\"') /* control PV */
				{
					nscan = sscanf( il+offset, "%[^\"]\"%n", namebuf, &offset2);
					offset += offset2;
					ctls.controlPV = namebuf;
				}
				else
				{
					ctls.controlPV = NULL;
					offset++;
				}

				if (gobble(il, "start:%lg%n", &ctls.startVal, &offset) == 0)
				{
					if (gobble(il, "start:\"%[^\"]\"%n", startbuf, &offset) != 0)
						ctls.startMacro = startbuf;
				}
				if (gobble(il, "delta:%lg%n", &ctls.deltaVal, &offset) == 0)
				{
					if( gobble(il, "delta:\"%[^\"]\"%n", deltabuf, &offset) != 0)
						ctls.deltaMacro = deltabuf;
				}
				if (gobble(il, "final:%lg%n", &ctls.finalVal, &offset) == 0)
				{
					if( gobble(il, "final:\"%[^\"]\"%n", finalbuf, &offset) != 0)
						ctls.finalMacro = finalbuf;
				}
				if(gobble(il, "active:%d%n", &active, &offset) == 0)
					active = 7;
				ctls.haveStartVal = (active&4)?1:0;
				ctls.haveDeltaVal = (active&2)?1:0;
				ctls.haveFinalVal = (active&1)?1:0;
				addScanControl( sc, &ctls);
				continue;
			}
			if((nscan = sscanf( il, "# Delay %[^:]:%n", typebuf, &offset)) > 0)
			{
				printf("DELAY configuration no longer valid\n");
				continue;
			}

			if(( nscan = sscanf( il, "# Trigger %[^:]: type:%d name:\"%[^\"]\" value:\"%[^\"]\"", typebuf, &id, namebuf, valuebuf)) > 0)
			{
				printf("TRIGGER configuration no longer valid\n");
				continue;
			}
			if(( nscan = sscanf( il, "# Action %[A-Za-z] %[A-Za-z] %n", typebuf, namebuf, &offset) ) > 1)
			{
				/* convert the different action lines */
				int idx, type, waitflag;
				double delay;
				idx = tgTypeIndex( typebuf);
				if( idx < 0)
					continue;
				type = getActionType( namebuf);
				if( type < 0)
					continue;
				switch( type)
				{
				case AA_DELAY_TIME:
					gobble( il, "%lf %n", &delay, &offset);
					new_acqAction_delayTime( &sc->actions[idx], delay, master);
					break;

				case AA_SET_PV:
					gobble(il, "\"%[^\"]\"%n", namebuf, &offset);
					gobble(il, "\"%[^\"]\"%n", valuebuf, &offset);
					new_acqAction_setPV( &sc->actions[idx], namebuf, valuebuf, master);
					break;

				case AA_WAIT_PV:
					gobble(il, "\"%[^\"]\"%n", namebuf, &offset);
					gobble(il, "\"%[^\"]\"%n", valuebuf, &offset);
					new_acqAction_waitPV( &sc->actions[idx], namebuf, valuebuf, master);
					break;

				case AA_WAIT_MOTOR:
					gobble(il, "\"%[^\"]\"%n", namebuf, &offset);
					new_acqAction_waitMotor( &sc->actions[idx], namebuf, master);
					break;

				case AA_CALL_EVENT:
					gobble(il, "\"%[^\"]\"%n", namebuf, &offset);
					gobble(il, "%d%n", &waitflag, &offset);
					new_acqAction_callEvent( &sc->actions[idx], namebuf, waitflag, master);
					break;

				case AA_CALL_SCAN:
					gobble(il, "\"%[^\"]\"%n", namebuf, &offset);
					gobble(il, "%d%n", &waitflag, &offset);
					new_acqAction_callScan( &sc->actions[idx], namebuf, waitflag, master);
					break;

				case AA_WAIT_EVENT:
					gobble(il, "\"%[^\"]\"%n", namebuf, &offset);
					new_acqAction_waitEvent( &sc->actions[idx], namebuf, master);
					break;

				case AA_WAIT_SCAN:
					gobble(il, "\"%[^\"]\"%n", namebuf, &offset);
					new_acqAction_waitScan( &sc->actions[idx], namebuf, master);
					break;

				case AA_SET_CONTROL:
					new_acqAction_setControl( &sc->actions[idx], master);
					break;

				case AA_NEXT_OUTPUT:
					new_acqAction_nextOutput( &sc->actions[idx], master);
					break;
				}

				continue;
			}
			inputState = IN_UNKNOWN;
			reuse_line = 1;
			continue;
			}

			case IN_EVENT:
			{
			int at, r0, rP, disable, ready, offset, isSpectrum;
			if( strncmp( "# datastream columns:", il, 20) == 0)	/* readability line */
				continue;
			if( sscanf( il, "# commentPrefix: %d", &id) == 1)
			{
				ev->putCommentPrefix = id;
				continue;
			}
			if( sscanf( il, "# eventID:%d AbsTime:%d Rel0Time:%d relPTime:%d", &id, &at, &r0, &rP) == 4)
			{
				ev->putEventId = (id!=0);
				ev->putAbsTime = (at!=0);
				ev->putRel0Time = (r0!=0);
				ev->putRelPTime = (rP!=0);
				continue;
			}
			if( sscanf( il, "# PV %d:%n",&id, &offset) == 1)
			{
				char format[200];
				format[0] = '\0';
				gobble( il, " \"%[^\"]\"%n", namebuf, &offset);
				gobble( il, " disable:%d%n", &disable,&offset);
				gobble( il, " format:\"%[^\"]\"%n", format, &offset);
				if( gobble( il, " spectrum:%d%n", &isSpectrum,&offset) != 1)
					isSpectrum = 0;

				if( gobble( il, " ready:%d%n", &ready, &offset) != 1)
					ready = useCurrent;

				/* note that the index is irrelevant: PV's are added in
				 * the order they appear.
				 */
				addEventPv( ev, namebuf, disable, format, ready, isSpectrum);
				continue;
			}

			inputState = IN_UNKNOWN;
			reuse_line = 1;
			continue;

			}

			case IN_MOTOR:
			{
				int offset = 0;
				char unused;
				if( sscanf( il, "# Motors:%c%n", &unused, &offset) > 0)
				{
					while( gobble(il, " \"%[^\"]\"%n", &namebuf, &offset) > 0)
						new_acqOneMotor( m, namebuf);

					continue;
				}

			}
			inputState = IN_UNKNOWN;
			reuse_line = 1;
			continue;

			/* happily reads and discards the old file information.
			 */
			case IN_FILE:
			{
				int val;

				if( sscanf( il, "# Template: \"%[^\"]\"", namebuf) > 0)
				{
					continue;
				}
				if( sscanf( il, "# Sequence: %d", &val) > 0)
				{
					continue;
				}
				if( sscanf( il, "# Header: %d", &val) > 0)
				{
					continue;
				}
				if( sscanf( il, "# SpectrumFormat: %d", &val) > 0)
				{
					continue;
				}
			}

			case IN_HANDLER:
			{
				char valuebuf[100];
				if( sscanf( il, "# Property \"%[^=]=\"%[^\"]\"", namebuf, valuebuf) > 1)
				{
				}
			}

			inputState = IN_UNKNOWN;
			reuse_line = 1;
			continue;
		}
	}

}

static int
gobble( char *input, char *pattern, void *addr, int *offset)
{
	int delta;
	int nscan;

	if( input[*offset] == '\0')
		return 0;

	/* suppress leading spaces */
	while( (input+*offset)[0] == ' ')
		(*offset)++;
	nscan = sscanf( input+*offset, pattern, addr, &delta);
	if( nscan == 0)
	{
		DEBUG printf("gobble '%s' pattern:'%s' addr:%p fails\n",
			input+*offset, pattern, addr);
		return 0;
	}
	DEBUG printf("gobble '%s' pattern:'%s' addr:%p delta:%d\n",
		input+*offset, pattern, addr, delta);
	*offset += delta;
	return nscan;
}
