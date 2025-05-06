#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "GeneratorConfigWidget.h"
#include "area.h"

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	void addPolygon(Polygon *polygon);
	void removePolygon(QUuid id);
	~MainWindow();

private slots:
	void toggleGeneratorDock();

private:
	Ui::MainWindow *ui;
	void newPolygon();
	void selectPolygon(QUuid id);

	void saveConfigs();

	GeneratorConfigWidget *generatorConfigWidget = nullptr;
};
#endif // MAINWINDOW_H
