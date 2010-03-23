/* $Header: epicsConnectApp/src/macro.c 1.3 2009/12/23 09:58:56CST Glen Wright (wrightg) Exp  $
 * Copyright Canadian Light Source, Inc. All rights reserved.
 *
 * Description:
 *    Manage string macros.
 *
 */

#include <stdlib.h>
#include <string.h>
#include "epicsConnect.h"
#include <stdio.h>

#define DEBUGP if(debugMacro)

int debugMacro;

static struct macroTable *default_macro_first;
static void add_macro( char*, char*, macroTable **);
static void macro_expand_n(const char * string, char *buffer, unsigned int buffer_size, macroTable *macroFirst ) ;

static macroTable *
createMacro( char *name, char *val, struct macroTable **macroFirst)
{
	macroTable *mt;

	mt = calloc( 1, sizeof *mt);
	mt->next = *macroFirst;
	*macroFirst = mt;
	mt->macroName = strdup(name);
	mt->macroVal = strdup(val);
	return mt;
}

macroTable *
find_macro(const char *name, const macroTable *macroFirst)
{
	const macroTable *mt;

	for(mt = macroFirst; mt ; mt = mt->next)
		if( strcmp( mt->macroName, name) == 0)
			return (macroTable *)mt;

	if( macroFirst != default_macro_first)
		return find_macro( name, default_macro_first);

	return NULL;
}

void
parse_macros( const char *macro_string, macroTable ** macro_first)
{
	char setMacro[200];
	const char *s, *e;		/* start and end in 'arg' of one macro assignment */
	char *m, *v;

	for( s=macro_string; s && *s ; s = e)
	{
		/* find a single comma delimited entry */
		e = s + strcspn(s, ",");
		if( e-s >= sizeof setMacro)
		{
			/* skip macros that don't fit */
			if( *e) e++;
			continue;
		}
		
		strncpy(setMacro, s, e-s);
		setMacro[e-s] = '\0';
		if( *e) e++;

		/* break an entry up into before and after '=' */
		m = setMacro;
		v = m + strcspn(m, "=");
		if(*v)
		{
			*v++ = '\0';
			if (*m)
				add_macro(m, v, macro_first);
		}
	}
}


static void
add_macro( char *name, char *val, macroTable **macroFirst )
{
	macroTable *mt;

	if( !name || !val || *name == '\0')
		return;

	if( macroFirst == NULL)
		macroFirst = &default_macro_first;

	mt = find_macro(name, *macroFirst);
	if( mt)
	{
		if(mt->macroVal)
			free(mt->macroVal);
		mt->macroVal = strdup(val);
		return;
	}
	mt = createMacro(name, val, macroFirst );
}

/*
 * 'limitless' call from outside.
 */
void
macro_expand( const char * string, char *buffer, macroTable *macroFirst )
{
	macro_expand_n(string, buffer, 10000, macroFirst);
}
/*
 * take a macro string, and expand it.
 * consider making this an exported function.
 */
static void
macro_expand_n(const char * string, char *buffer, unsigned int buffer_size, macroTable *macroFirst ) 
{
	const char *sm, *em;	/* start macro, end macro */
	char *sx;	/* start expand */
	char macro_name[100];
	char nested_expand[100];
	macroTable *mt;
	int buffer_remain = buffer_size;

	DEBUGP printf("macro_expand_n( '%s', %p, %d, %p)\n", string, buffer, buffer_size, macroFirst);

	sm = string;
	sx = buffer;

	if( macroFirst == NULL)
		macroFirst = default_macro_first;

	while (*sm)
	{
		if( *sm != '$' || sm[1] != '(' )
		{
			if( buffer_remain > 1)
			{
				*sx++ = *sm++;
				buffer_remain--;
			}
			continue;
		}
		sm += 2;
		em = sm + strcspn(sm, "$)");

		if( *em == '\0')	/* unterminated macro -- ignored */
			break;

		/* nested expand: take the contents of the macro name at this level, and expand it */
		if( *em == '$' && em[1] == '(')
		{
			int nest = 1;
			for ( ; *em && nest > 0 ; em++)
			{
				if( *em == '(')
					nest++;
				else
				if( *em == ')')
					nest--;
			}
			strncpy(nested_expand, sm, em-sm);			/* get the unexpanded macro name */
			nested_expand[em-sm-1] = '\0';
			macro_expand_n(nested_expand, macro_name, sizeof macro_name, macroFirst);	/* recurse to determine macro name */
		}
		else
		if( *em == ')')
		{
			/* simple macro expansion */
			strncpy(macro_name, sm, em-sm);
			macro_name[em-sm] = '\0';
		}
		mt = find_macro(macro_name, macroFirst);
		if( mt == NULL)
		{
			/* if no macro, replace with the macro call */
			if( buffer_remain > 2)
			{
				buffer_remain -= 2;
				*sx++ = '$';
				*sx++ = '(';
			}
			continue;
		}
		/* handle recursive macros */
		if( mt->expand)
			continue;
		mt->expand++;
		macro_expand_n(mt->macroVal, sx, buffer_remain, macroFirst);
		mt->expand--;
		while(*sx)	/* skip over the expansion string */
		{
			buffer_remain--;
			sx++;
		}
		sm = em+1;
		continue;
	}
	if( buffer_remain > 0)
		*sx++ = '\0';
	DEBUGP printf("macro_expand_n() '%s' => '%s'\n", string, buffer);
}

/* return the default macro head */
macroTable *macro_head()
{
	return default_macro_first;
}

/* set a macro value */
void set_macroValue(macroTable *macrop, const char *value)
{
	if( macrop == NULL)
		return;
	if( macrop->macroVal)
		free(macrop->macroVal);
	macrop->macroVal = strdup(value);
}


/* 
 * $Log: epicsConnectApp/src/macro.c  $
 * Revision 1.3 2009/12/23 09:58:56CST Glen Wright (wrightg) 
 * Compilation messages cleanup;
 * more cautious cleanup on removal of a connector and channel;
 * bug fix when monitor callback structure doesn't have ECA_NORMAL as a status.
 * Revision 1.2 2008/12/03 14:30:43CST Glen Wright (wrightg) 
 * 
 * Revision 1.1 2008/05/08 10:19:21CST David Beauregard (beaured0) 
 * Initial revision
 * Member added to project e:/MKS_Home/archive/cs/epics_local/epics_tools/epicsConnect/project.pj
 * Revision 1.3 2008/03/27 11:14:47CST Glen Wright (wrightg) 
 * Exposed more methods for the macro table.
 * Revision 1.2 2007/05/19 12:37:13CST Elder Matias (matiase) 
 * Added header/log
 *
 */

