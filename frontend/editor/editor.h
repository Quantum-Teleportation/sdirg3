#ifndef EDITOR_H
#define EDITOR_H

#include "geometry.h"
#include "mainwindow.h"
#include <QDockWidget>
#include <QPushButton>
#include <QShowEvent>
#include <QTableWidget>
#include <QVector2D>
#include <QWidget>

namespace Ui {
class Editor;
}

class Editor : public QWidget {
	Q_OBJECT

public:
	explicit Editor(QWidget *parent = nullptr);
	// void setupNew();
	void setupExistingPolygon(Polygon *polygon);
	// void resetEditor();
	~Editor();

protected:
	void showEvent(QShowEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;

private slots:
	void onBufferConnect();
	void onPolygonSelectReceived(Polygon *polygon);

	void addVertex();
	void load(); // testing

	void addVertexByMouse(QPointF point); // add new point
	void editVertexCoord(
		int row,
		QPointF new_coord); // change coord for vertex when move by mouse

private:
	void updateTableSize();
	// void addVertexRow(int row, QString vName, double x, double y);
	void addVRow(int row, QString vName, double x, double y);
	void saveVertex(int row);
	void resetVertex(int row);
	void finishEditVertex(int row);
	void editVertex(int row);
	void clearNew();
	void savePolygon();
	void resetEditor();

	Ui::Editor *ui;
	QTableWidget *vtable;
	QPushButton *cancelBtn = nullptr;
	; // костыль
	QTableWidget *etable;
	QVector<QVector2D> buffer;
	QDockWidget *dock;
	MainWindow *mainWindow;
	int polygonNumber;
	Polygon *editedPolygon = nullptr;
	QIcon addIcon = style()->standardIcon(QStyle::SP_DialogOkButton);
	QIcon editIcon = style()->standardIcon(QStyle::SP_ArrowBack);
	QIcon saveIcon = style()->standardIcon(QStyle::SP_DialogApplyButton);
	QIcon resetIcon = style()->standardIcon(QStyle::SP_DialogCancelButton);

	const double minCoord = 0.0;
	const double maxCoord = 1000.0;
	const int precision = 3;
};

#endif // EDITOR_H
