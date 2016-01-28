#include "summoners.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Summoners w;
	w.show();
	return a.exec();
}
