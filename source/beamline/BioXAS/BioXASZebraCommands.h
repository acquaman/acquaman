#ifndef BIOXASZEBRACOMMANDS
#define BIOXASZEBRACOMMANDS

#include <QString>

namespace BioXASZebraCommand
{
	/// Helper command function.  If given a source name, it returns the number.  Number is required for all source settings.
	inline int commandFromName(const QString &commandName)
	{
		int command = -1;

		if (commandName == "DISCONNECT")
			command = 0;

		else if (commandName == "IN1 TTL")
			command = 1;

		else if (commandName == "IN1 NIM")
			command = 2;

		else if (commandName == "IN1 LVDS")
			command = 3;

		else if (commandName == "IN2 TTL")
			command = 4;

		else if (commandName == "IN2 NIM")
			command = 5;

		else if (commandName == "IN2 LVDS")
			command = 6;

		else if (commandName == "IN3 TTL")
			command = 7;

		else if (commandName == "IN3 OC")
			command = 8;

		else if (commandName == "IN3 LVDS")
			command = 9;

		else if (commandName == "IN4 TTL")
			command = 10;

		else if (commandName == "IN4 CMP")
			command = 11;

		else if (commandName == "IN4 PECL")
			command = 12;

		else if (commandName == "IN5 ENCA")
			command = 13;

		else if (commandName == "IN5 ENCB")
			command = 14;

		else if (commandName == "IN5 ENCZ")
			command = 15;

		else if (commandName == "IN5 CONN")
			command = 16;

		else if (commandName == "IN6 ENCA")
			command = 17;

		else if (commandName == "IN6 ENCB")
			command = 18;

		else if (commandName == "IN6 ENCZ")
			command = 19;

		else if (commandName == "IN6 CONN")
			command = 20;

		else if (commandName == "IN7 ENCA")
			command = 21;

		else if (commandName == "IN7 ENCB")
			command = 22;

		else if (commandName == "IN7 ENCZ")
			command = 23;

		else if (commandName == "IN7 CONN")
			command = 24;

		else if (commandName == "IN8 ENCA")
			command = 25;

		else if (commandName == "IN8 ENCB")
			command = 26;

		else if (commandName == "IN8 ENCZ")
			command = 27;

		else if (commandName == "IN8 CONN")
			command = 28;

		else if (commandName == "PC ARM")
			command = 29;

		else if (commandName == "PC GATE")
			command = 30;

		else if (commandName == "PC PULSE")
			command = 31;

		else if (commandName == "AND1")
			command = 32;

		else if (commandName == "AND2")
			command = 33;

		else if (commandName == "AND3")
			command = 34;

		else if (commandName == "AND4")
			command = 35;

		else if (commandName == "OR1")
			command = 36;

		else if (commandName == "OR2")
			command = 37;

		else if (commandName == "OR3")
			command = 38;

		else if (commandName == "OR4")
			command = 39;

		else if (commandName == "GATE1")
			command = 40;

		else if (commandName == "GATE2")
			command = 41;

		else if (commandName == "GATE3")
			command = 42;

		else if (commandName == "GATE4")
			command = 43;

		else if (commandName == "DIV1 OUTD")
			command = 44;

		else if (commandName == "DIV2 OUTD")
			command = 45;

		else if (commandName == "DIV3 OUTD")
			command = 46;

		else if (commandName == "DIV4 OUTD")
			command = 47;

		else if (commandName == "DIV1 OUTN")
			command = 48;

		else if (commandName == "DIV2 OUTN")
			command = 49;

		else if (commandName == "DIV3 OUTN")
			command = 50;

		else if (commandName == "DIV4 OUTN")
			command = 51;

		else if (commandName == "PULSE1")
			command = 52;

		else if (commandName == "PULSE2")
			command = 53;

		else if (commandName == "PULSE3")
			command = 54;

		else if (commandName == "PULSE4")
			command = 55;

		else if (commandName == "QUAD OUTA")
			command = 56;

		else if (commandName == "QUAD OUTB")
			command = 57;

		else if (commandName == "CLOCK 1KHZ")
			command = 58;

		else if (commandName == "CLOCK 1MHZ")
			command = 59;

		else if (commandName == "SOFT IN1")
			command = 60;

		else if (commandName == "SOFT IN2")
			command = 61;

		else if (commandName == "SOFT IN3")
			command = 62;

		else if (commandName == "SOFT IN4")
			command = 63;

		return command;
	}

	/// Helper command function.  If given a source name, it returns the number.  Number is required for all source settings.
	inline QString nameFromCommand(int command)
	{
		QString commandName = "";

		switch(command){

		case 0:
			commandName = "DISCONNECT";
			break;

		case 1:
			commandName = "IN1 TTL";
			break;

		case 2:
			commandName = "IN1 NIM";
			break;

		case 3:
			commandName = "IN1 LVDS";
			break;

		case 4:
			commandName = "IN2 TTL";
			break;

		case 5:
			commandName = "IN2 NIM";
			break;

		case 6:
			commandName = "IN2 LVDS";
			break;

		case 7:
			commandName = "IN3 TTL";
			break;

		case 8:
			commandName = "IN3 OC";
			break;

		case 9:
			commandName = "IN3 LVDS";
			break;

		case 10:
			commandName = "IN4 TTL";
			break;

		case 11:
			commandName = "IN4 CMP";
			break;

		case 12:
			commandName = "IN4 PECL";
			break;

		case 13:
			commandName = "IN5 ENCA";
			break;

		case 14:
			commandName = "IN5 ENCB";
			break;

		case 15:
			commandName = "IN5 ENCZ";
			break;

		case 16:
			commandName = "IN5 CONN";
			break;

		case 17:
			commandName = "IN6 ENCA";
			break;

		case 18:
			commandName = "IN6 ENCB";
			break;

		case 19:
			commandName = "IN6 ENCZ";
			break;

		case 20:
			commandName = "IN6 CONN";
			break;

		case 21:
			commandName = "IN7 ENCA";
			break;

		case 22:
			commandName = "IN7 ENCB";
			break;

		case 23:
			commandName = "IN7 ENCZ";
			break;

		case 24:
			commandName = "IN7 CONN";
			break;

		case 25:
			commandName = "IN8 ENCA";
			break;

		case 26:
			commandName = "IN8 ENCB";
			break;

		case 27:
			commandName = "IN8 ENCZ";
			break;

		case 28:
			commandName = "IN8 CONN";
			break;

		case 29:
			commandName = "PC ARM";
			break;

		case 30:
			commandName = "PC GATE";
			break;

		case 31:
			commandName = "PC PULSE";
			break;

		case 32:
			commandName = "AND1";
			break;

		case 33:
			commandName = "AND2";
			break;

		case 34:
			commandName = "AND3";
			break;

		case 35:
			commandName = "AND4";
			break;

		case 36:
			commandName = "OR1";
			break;

		case 37:
			commandName = "OR2";
			break;

		case 38:
			commandName = "OR3";
			break;

		case 39:
			commandName = "OR4";
			break;

		case 40:
			commandName = "GATE1";
			break;

		case 41:
			commandName = "GATE2";
			break;

		case 42:
			commandName = "GATE3";
			break;

		case 43:
			commandName = "GATE4";
			break;

		case 44:
			commandName = "DIV1 OUTD";
			break;

		case 45:
			commandName = "DIV2 OUTD";
			break;

		case 46:
			commandName = "DIV3 OUTD";
			break;

		case 47:
			commandName = "DIV4 OUTD";
			break;

		case 48:
			commandName = "DIV1 OUTN";
			break;

		case 49:
			commandName = "DIV2 OUTN";
			break;

		case 50:
			commandName = "DIV3 OUTN";
			break;

		case 51:
			commandName = "DIV4 OUTN";
			break;

		case 52:
			commandName = "PULSE1";
			break;

		case 53:
			commandName = "PULSE2";
			break;

		case 54:
			commandName = "PULSE3";
			break;

		case 55:
			commandName = "PULSE4";
			break;

		case 56:
			commandName = "QUAD OUTA";
			break;

		case 57:
			commandName = "QUAD OUTB";
			break;

		case 58:
			commandName = "CLOCK 1KHZ";
			break;

		case 59:
			commandName = "CLOCK 1MHZ";
			break;

		case 60:
			commandName = "SOFT IN1";
			break;

		case 61:
			commandName = "SOFT IN2";
			break;

		case 62:
			commandName = "SOFT IN3";
			break;

		case 63:
			commandName = "SOFT IN4";
			break;
		}

		return commandName;
	}
}

#endif // BIOXASZEBRACOMMANDS

