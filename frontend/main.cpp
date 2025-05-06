#include "mainwindow.h"

#include <QApplication>
#include <generator.h>

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	terraformer::generator gen;
	gen.saveAs("test.conf");

	return a.exec();
}
