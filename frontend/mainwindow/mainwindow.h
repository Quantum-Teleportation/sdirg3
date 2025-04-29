#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

private:
	Ui::MainWindow *ui;
	void newPolygon();
	void selectPolygon(QUuid id);

	void saveConfigs();
	// Editor editor;
};
#endif // MAINWINDOW_H
