#include "client.h"
#include "../MessageDefine.h"
#include "SWSLog.h"

Client::Client(QObject *parent, bool bDeny)
	: QTcpSocket(parent), bDenyLogin(bDeny), bWasLogin(false)
{
	startTimer(1000);
	heartbeat = timeoutTime;
	bFighting = false;
	
	player = {
		"testUser", 1, 0, 100, 5
	};

	connect(this, SIGNAL(readyRead()), this, SLOT(receiveMessage()));
	connect(this, SIGNAL(disconnected()), this, SLOT(ClientClose()));
	connect(this, SIGNAL(error(QAbstractSocket::SocketError)), 
		this, SLOT(Socketerror(QAbstractSocket::SocketError)));
}

Client::~Client()
{

}

void Client::ClientClose()
{
	qDebug() << "client disconnected.\n";
	emit CloseSignal(socketDescriptor());
	deleteLater();
}

void Client::Socketerror(QAbstractSocket::SocketError socketError)
{
	LogIns.FlashLog("Client:%d An error occurred. Code:%d\n", socketDescriptor(), socketError);
}

void Client::receiveMessage()
{
	qint32 msgID;
	QDataStream oStream(this->readAll());
	oStream.setByteOrder(QDataStream::LittleEndian);
	
	while (!oStream.atEnd())
	{
		oStream >> msgID;

		if (msgID != CS_HEART) {
			qDebug() << "";
			qDebug() << QTime::currentTime().toString("HH:mm:ss") << "Received message(exclusive HEART), ID:" << msgID;
		}

		if (!bWasLogin && msgID != CS_Login)
		{		
			return;	//未登陆时，只处理Login消息。
		}

		switch (msgID)
		{
		case CS_HEART:
			//qDebug() << tr("%1 ..^..").arg(socketDescriptor());
			heartbeat = 0;
			break;
		case CS_Login:		msg_login(oStream);			break;
		case CS_RoleInfo:	msg_CS_RoleInfo(oStream);	break;
		case CS_FightStart:	msg_CS_FightStart(oStream);	break;
		case CS_Action:		msg_CS_Action(oStream);		break;
		default:
			break;
		}
	}
}

void Client::timerEvent(QTimerEvent *t)
{
	++heartbeat;
}

void Client::msg_login(QDataStream &oStream)
{
	bWasLogin = true;
	QByteArray iData;
	QDataStream iStream(&iData, QIODevice::WriteOnly);
	iStream.setByteOrder(QDataStream::LittleEndian);
	iStream << SC_Login << bWasLogin;
	write(iData);

	qDebug() << __FUNCTION__;
}
void Client::msg_CS_RoleInfo(QDataStream &oStream)
{
	QByteArray iData;
	QDataStream iStream(&iData, QIODevice::WriteOnly);
	iStream.setByteOrder(QDataStream::LittleEndian);

	int32_t len = player.name.length();

	iStream << SC_RoleInfo << len;
	iStream.writeRawData(player.name.toStdString().c_str(), len);
	iStream << player.level << player.exp << player.hp << player.dc;
	write(iData);

	qDebug() << __FUNCTION__ << len << player.name << player.level << player.exp << player.hp << player.dc;
}

void Client::msg_CS_FightStart(QDataStream &oStream)
{
	QByteArray iData;
	QDataStream iStream(&iData, QIODevice::WriteOnly);
	iStream.setByteOrder(QDataStream::LittleEndian);

	static qint32 num = 0;
	qint32 level = 5 * qrand() / RAND_MAX + player.level / 2;
	monster = {
		QStringLiteral("monster%1").arg(num),
		level, level, level * 5, level / 2 + 1
	};

	qint32 len = monster.name.length();
	const char *name = monster.name.toStdString().c_str();
	iStream << SC_FightStart << len;
	iStream.writeRawData(monster.name.toStdString().c_str(), len);
	iStream << monster.level << monster.exp << monster.hp << monster.dc;
	write(iData);

	++num;
	bFighting = true;
	qDebug() << __FUNCTION__ << len << name << monster.level << monster.exp << monster.hp << monster.dc;
}

void Client::msg_CS_Action(QDataStream &oStream)
{
	QByteArray iData;
	QDataStream iStream(&iData, QIODevice::WriteOnly);
	iStream.setByteOrder(QDataStream::LittleEndian);

	qint32 nTmp;
	ActionType at;
	oStream >> nTmp;
	at = static_cast<ActionType>(nTmp);

	switch (at)
	{
	case at_Attack:
		monster.hp -= player.dc;
		player.hp -= monster.dc;
		break;
	case at_Defense:
		//假设格档强制掉血1点
		player.hp -= 1;
		break;
	case at_Recovery:
		//假设每次恢复50点血，并且不考虑血量上限
		player.hp += 50;
		player.hp -= monster.dc;
		break;
	default:
		break;
	}

	iStream << SC_Action << player.hp << monster.hp;
	write(iData);

	checkFightState();	
	qDebug() << __FUNCTION__ << at << player.hp << monster.hp;
}

void Client::checkFightState(void)
{
	qint32 nTmp = FR_unknow;

	QByteArray iData;
	QDataStream iStream(&iData, QIODevice::WriteOnly);
	iStream.setByteOrder(QDataStream::LittleEndian);

	if (monster.hp <= 0)
	{
		nTmp = FR_victory;
		iStream << SC_FightFinish << nTmp;
	}
	else if (player.hp <= 0)
	{
		nTmp = FR_fail;
		iStream << SC_FightFinish << nTmp;
	};
	write(iData);
	bFighting = false;

	if (nTmp == FR_victory)
	{
		CalcIncome();
	}

	qDebug() << __FUNCTION__ << nTmp << player.hp << monster.hp;
}

void Client::CalcIncome(void)
{
	player.exp += monster.exp;
	if (player.exp >= player.level * 30)
	{
		//假设每级的升级经验 = level * 30
		player.level += 1;
		player.exp = 0;
		player.hp = 100 + player.level * 30;
	}

	//正确处理流程应该是给自己发送一条消息。直接采用此方式调用，存在隐患。
	msg_CS_RoleInfo(QDataStream());

	qDebug() << __FUNCTION__ << player.level << player.exp << player.hp;
}