#ifndef _ENVIRONMENT_H
#define _ENVIRONMENT_H

enum ActionType
{
	at_Attack,
	at_Defense,
	at_Recovery,
};

enum FightResult
{
	FR_unknow,
	FR_victory,
	FR_fail,
	FR_draw,
};

struct Organisms
{
	QString name;
	qint32 level;
	qint32 exp;
	qint32 hp;
	qint32 dc;
};

#endif //#ifndef _ENVIRONMENT_H