#ifndef _MESSAGEDEFINE_H
#define _MESSAGEDEFINE_H

enum MessageType
{
	HEART,
	LOGIN,
	LOGOUT,
	RTSP_START = 50,
	RTSP_STOP,
	GET_CHANNELCOUNT,
	GET_OUTPUT = 100,
	GET_INPUTSCALE,
	GET_DISTORT,
	GET_INPUTCOUNT,
	GET_INPUT,
	SET_OUTPUT = 150,
	SET_INPUTSCALE,
	SET_DISTORT,
	SET_INPUT,
	ADD_CHANNEL = 200,
	ADD_INPUT,
	DEL_CHANNEL = 250,
	DEL_INPUT,

	ACK_HEART = 500,
	ACK_LOGIN,
	ACK_LOGOUT,
	ACK_RTSP_START = 550,
	ACK_RTSP_STOP,
	ACK_GET_CHANNELCOUNT,
	ACK_GET_OUTPUT = 600,
	ACK_GET_INPUTSCALE,
	ACK_GET_DISTORT,
	ACK_GET_INPUTCOUNT,
	ACK_GET_INPUT,
	ACK_SET_OUTPUT = 650,
	ACK_SET_INPUTSCALE,
	ACK_SET_DISTORT,
	ACK_SET_INPUT,
	ACK_ADD_CHANNEL = 700,
	ACK_ADD_INPUT,
	ACK_DEL_CHANNEL = 750,
	ACK_DEL_INPUT,

	MaxMessage
};

#endif //#ifndef _MESSAGEDEFINE_H