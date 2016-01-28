#ifndef _MESSAGEDEFINE_H
#define _MESSAGEDEFINE_H

enum MessageType
{
	CS_HEART,
	CS_Login,
	CS_RoleInfo,
	CS_FightStart,
	CS_Action,
	
	MaxC2SMessage,

	MinS2CMessage = 500,
	SC_HEART = MinS2CMessage,
	SC_Login,
	SC_RoleInfo,
	//SC_updateRole,
	SC_FightStart,
	SC_FightFinish,
	SC_Action,
	
	MaxS2CMessage,
};

#endif //#ifndef _MESSAGEDEFINE_H