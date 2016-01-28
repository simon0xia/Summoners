#include "summoners.h"
#include "../MessageDefine.h"
#include "../environment.h"
#include <QHostAddress>

Summoners::Summoners(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	ui.edit_display_msg->document()->setMaximumBlockCount(50);
	ui.edit_display->document()->setMaximumBlockCount(50);
}

Summoners::~Summoners()
{

}


void Summoners::on_btn_login_clicked()
{
	quint32 port = ui.edit_port->text().toUInt();
	QHostAddress addr(ui.edit_addr->text());

	Socket = new QTcpSocket;
	Socket->connectToHost(addr, port);

	connect(Socket, SIGNAL(connected()), this, SLOT(processConnection()));
	connect(Socket, SIGNAL(readyRead()), this, SLOT(receiveMessage()));

	timer_heart = startTimer(1000);		//假设每秒1次心跳。不同的游戏，心跳间隔不一样。

	QByteArray datagram;
	QDataStream iStream(&datagram, QIODevice::WriteOnly);
	iStream.setByteOrder(QDataStream::LittleEndian);
	iStream << CS_Login;
	Socket->write(datagram);
}

void Summoners::on_btn_startFight_clicked(void)
{
	QByteArray datagram;
	QDataStream iStream(&datagram, QIODevice::WriteOnly);
	iStream.setByteOrder(QDataStream::LittleEndian);
	iStream << CS_FightStart;
	Socket->write(datagram);
}
void Summoners::on_btn_quitFight_clicked(void)
{

}

void Summoners::on_btn_attack_clicked(void)
{
	QByteArray datagram;
	QDataStream iStream(&datagram, QIODevice::WriteOnly);
	iStream.setByteOrder(QDataStream::LittleEndian);
	iStream << CS_Action << (qint32)at_Attack;
	Socket->write(datagram);
}
void Summoners::on_btn_defense_clicked(void)
{
	QByteArray datagram;
	QDataStream iStream(&datagram, QIODevice::WriteOnly);
	iStream.setByteOrder(QDataStream::LittleEndian);
	iStream << CS_Action << (qint32)at_Defense;
	Socket->write(datagram);
}
void Summoners::on_btn_recovery_clicked(void)
{
	QByteArray datagram;
	QDataStream iStream(&datagram, QIODevice::WriteOnly);
	iStream.setByteOrder(QDataStream::LittleEndian);
	iStream << CS_Action << (qint32)at_Recovery;
	Socket->write(datagram);
}

//理论上这里收到消息之后，要弄个hashmap做消息映射。采用这种switch办法很难看，还导致这个类处理太多无关的事情。
void Summoners::receiveMessage()
{
	QDataStream oStream(Socket->readAll());
	oStream.setByteOrder(QDataStream::LittleEndian);
	qint32 msgID;
	while (!oStream.atEnd())
	{
		oStream >> msgID;

		ui.edit_display_msg->append(QString::number(msgID));

		switch (msgID)
		{
		case SC_HEART:
			//can not receive this message.
			break;
		case SC_Login:		msg_login(oStream);		break;
		case SC_RoleInfo:  	msg_RoleInfo(oStream);	break;
		//case SC_updateRole:	msg_updateRole(oStream);break;
		case SC_FightStart:	msg_FightStart(oStream);break;
		case SC_FightFinish:msg_FightFinish(oStream);break;
		case SC_Action:		msg_Action(oStream);	break;

		default:
			break;
		}
	}
}

void Summoners::processConnection()
{
	//nothing
}

void Summoners::timerEvent(QTimerEvent *t)
{
	if (t->timerId() == timer_heart)
	{
		SendHeart();
	}
}

bool Summoners::SendHeart(void)
{
	QByteArray datagram;
	QDataStream iStream(&datagram, QIODevice::WriteOnly);
	iStream.setByteOrder(QDataStream::LittleEndian);
	iStream << CS_HEART;
	Socket->write(datagram);

	return true;
}

void Summoners::DisplayPlayerInfo(void)
{
	ui.lbl_role_name->setText(player.name);
	ui.lbl_role_level->setText(QString::number(player.level));
	ui.lbl_role_exp->setText(QString::number(player.exp));
	ui.lbl_role_hp->setText(QString::number(player.hp));
	ui.lbl_role_dc->setText(QString::number(player.dc));
}
void Summoners::DisplayMonsterInfo(void)
{
	ui.lbl_monster_name->setText(monster.name);
	ui.lbl_monster_level->setText(QString::number(monster.level));
	ui.lbl_monster_exp->setText(QString::number(monster.exp));
	ui.lbl_monster_hp->setText(QString::number(monster.hp));
	ui.lbl_monster_dc->setText(QString::number(monster.dc));
}

void Summoners::msg_login(QDataStream &oStream)
{
	bool bRes;
	QString strTmp;
	qint32 Reason;
	oStream >> bRes;
	if (bRes)
	{
		strTmp += " login success.";
	}
	else
	{
		strTmp += " login fail. reason:";
		oStream >> Reason;
		if (Reason == 1) {
			strTmp += "username or password Validation failed.";
		} else if (Reason == 2){
			strTmp += "client limit.";
		} else {
			strTmp += "unknow";
		}
	}
	ui.edit_display->setText(strTmp);

	if (bRes)
	{
		QByteArray datagram;
		QDataStream iStream(&datagram, QIODevice::WriteOnly);
		iStream.setByteOrder(QDataStream::LittleEndian);
		iStream << CS_RoleInfo;
		Socket->write(datagram);
	}
}
void Summoners::msg_RoleInfo(QDataStream &oStream)
{
	qint32 len;
	char name[128] = {'\0'};	//假设名字最长128字符

	oStream >> len;
	oStream.readRawData(name, len);
	oStream >> player.level >> player.exp >> player.hp >> player.dc;
	player.name = name;

	DisplayPlayerInfo();
}
void Summoners::msg_updateRole(QDataStream &oStream)
{

}
void Summoners::msg_FightStart(QDataStream &oStream)
{
	qint32 len;
	char name[128] = { '\0' };	//假设名字最长128字符

	oStream >> len;
	oStream.readRawData(name, len);
	oStream >> monster.level >> monster.exp >> monster.hp >> monster.dc;
	monster.name = name;

	DisplayMonsterInfo();

	ui.btn_startFight->setEnabled(false);
	ui.btn_attack->setEnabled(true);
	ui.btn_defense->setEnabled(true);
	ui.btn_recovery->setEnabled(true);
}
void Summoners::msg_FightFinish(QDataStream &oStream)
{
	int nTmp;
	FightResult fr;
	oStream >> nTmp;

	fr = static_cast<FightResult>(nTmp);
	switch (fr)
	{
	case FR_unknow:
		ui.edit_display->setText(QStringLiteral("战斗信息错误"));
		break;
	case FR_victory:
		ui.edit_display->setText(QStringLiteral("胜利"));
		break;
	case FR_fail:
		ui.edit_display->setText(QStringLiteral("失败"));
		break;
	case FR_draw:
		ui.edit_display->setText(QStringLiteral("超时"));
		break;
	default:
		break;
	}

	ui.btn_startFight->setEnabled(true);
	ui.btn_attack->setEnabled(false);
	ui.btn_defense->setEnabled(false);
	ui.btn_recovery->setEnabled(false);
}
void Summoners::msg_Action(QDataStream &oStream)
{
	oStream >> player.hp >> monster.hp;

	ui.lbl_role_hp->setText(QString::number(player.hp));
	ui.lbl_monster_hp->setText(QString::number(monster.hp));
}