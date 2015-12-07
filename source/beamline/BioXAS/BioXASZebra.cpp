#include "BioXASZebra.h"

BioXASZebra::BioXASZebra(const QString &baseName, QObject *parent)
	: QObject(parent)
{

}

BioXASZebra::~BioXASZebra()
{

}

int BioXASZebra::commandFromName(const QString &commandName) const
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

