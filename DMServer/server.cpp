#include "server.h"
#include "SWSLog.h"

Server::Server(QObject *parent)
	: QTcpServer(parent)
{
	QSettings *ConfigIni = new QSettings("./Settings.ini", QSettings::IniFormat, this);
	port = ConfigIni->value("/COMM/Port", 7000).toInt();
	maxClient = ConfigIni->value("/COMM/MaxClient", 1).toInt();
	delete ConfigIni;

	ClientCount = 0;

	if (!listen(QHostAddress::Any, port))
	{
		qDebug() << tr("SWServer Server"), tr("Unable to start the server: %1.").arg(errorString());
		return;
	}

	QString ipAddress;
	QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
	// use the first non-localhost IPv4 address
	for (int i = 0; i < ipAddressesList.size(); ++i) {
		if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
			ipAddressesList.at(i).toIPv4Address()) {
			ipAddress = ipAddressesList.at(i).toString();
			break;
		}
	}

	// if we did not find one, use IPv4 localhost
	if (ipAddress.isEmpty())
		ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
	qDebug() << (tr("The server is running on\nIP: %1\tport: %2\n")
		.arg(ipAddress).arg(serverPort()));

	startTimer(1000);
}

Server::~Server()
{

}

void Server::incomingConnection(qintptr handle)
{
	Client *client = new Client(this, ClientCount >= maxClient);
	connect(client, SIGNAL(CloseSignal(qintptr)), this, SLOT(clientDisconnected(qintptr)));
	connect(this, SIGNAL(error(QAbstractSocket::SocketError)),
		this, SLOT(Socketerror(QAbstractSocket::SocketError)));

	++ClientCount;
	ClientList.append(client);
	client->setSocketDescriptor(handle);
}

void Server::clientDisconnected(qintptr descriptor)
{
	--ClientCount;
	for (int i = 0; i < ClientList.count(); i++)
	{
		QTcpSocket *item = ClientList.at(i);
		if (item->socketDescriptor() == descriptor)
		{
			ClientList.removeAt(i);
			return;
		}
	}
}

void Server::Socketerror(QAbstractSocket::SocketError socketError)
{
	LogIns.FlashLog("server has an error. Code:%d\n", socketDescriptor(), socketError);
}