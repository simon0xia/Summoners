
#include <QCoreApplication>
#include "server.h"
#include "SWSLog.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	//ȷ��ֻ����һ��
	QSystemSemaphore sema("ServerKey", 1, QSystemSemaphore::Open);
	//���ٽ������������ڴ�
	sema.acquire();
	QSharedMemory mem("ServerObject");

	// ���ȫ�ֶ����Դ������˳�
	if (!mem.create(1))
	{
		qDebug() << "A server has already been running.";
		sema.release();
		return 0;
	}
	sema.release();

	LogIns.SetLogPath();
	LogIns.FlashLog("DMS start.\n");
	Server server = new Server(NULL);

	return a.exec();
}
