#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "GeneratorConfigWidget.h"
#include "area.h"

#include <generator.h>

#include <QMainWindow>
#include <QScrollArea>

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
	void create_material_files();

private:
	Ui::MainWindow *ui;

	void newPolygon();
	void selectPolygon(QUuid id);

	GeneratorConfigWidget *generatorConfigWidget = nullptr;
	QScrollArea* 	scrollArea = nullptr;
};
#endif // MAINWINDOW_H
