#ifndef SUMMONERS_H
#define SUMMONERS_H

#include <QtWidgets/QMainWindow>
#include "ui_summoners.h"
#include <QTimerEvent>
#include <QtNetwork>
#include "../environment.h"

class Summoners : public QMainWindow
{
	Q_OBJECT

public:
	Summoners(QWidget *parent = 0);
	~Summoners();

private slots:
	void processConnection();
	void receiveMessage();

	void on_btn_login_clicked(void);
	void on_btn_startFight_clicked(void);
	void on_btn_quitFight_clicked(void);

	void on_btn_attack_clicked(void);
	void on_btn_defense_clicked(void);
	void on_btn_recovery_clicked(void);

	void msg_login(QDataStream &oStream);
	void msg_RoleInfo(QDataStream &oStream);
	void msg_updateRole(QDataStream &oStream);
	void msg_FightStart(QDataStream &oStream);
	void msg_FightFinish(QDataStream &oStream);
	void msg_Action(QDataStream &oStream);

private:
	void timerEvent(QTimerEvent *);

	bool SendHeart(void);
	void DisplayPlayerInfo(void);
	void DisplayMonsterInfo(void);

private:
	Ui::SummonersClass ui;

	qint32 timer_heart;
	QTcpSocket *Socket;

	Organisms player, monster;
};

#endif // SUMMONERS_H
