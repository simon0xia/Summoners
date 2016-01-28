#ifndef CLIENT_H
#define CLIENT_H

#include <QtNetwork>
#include "../environment.h"

class Client : public QTcpSocket
{
	Q_OBJECT

	const qint32 timeoutTime = 5;

public:
	Client(QObject *, bool);
	~Client();

signals:
	void CloseSignal(qintptr);

private slots:
	void receiveMessage();
	void ClientClose();
	void Socketerror(QAbstractSocket::SocketError);

private:
	void timerEvent(QTimerEvent *);

	void msg_login(QDataStream &oStream);
	void msg_CS_RoleInfo(QDataStream &oStream);
	void msg_CS_FightStart(QDataStream &oStream);
	void msg_CS_Action(QDataStream &oStream);

	void checkFightState(void);
	void CalcIncome(void);

private:
	bool bDenyLogin, bFighting, bWasLogin;
	qint32 heartbeat;

	Organisms player, monster;
};

#endif // CLIENT_H
