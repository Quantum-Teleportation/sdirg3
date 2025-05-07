#ifndef AREA_H
#define AREA_H

#include "geometry.h"

#include <QMouseEvent>
#include <QWidget>

class Mediator : public QObject {
	Q_OBJECT
public:
	static Mediator *instance() {
		static Mediator *mediatorInstance = new Mediator();
		return mediatorInstance;
	}

signals:
	void onBufferConnect(QVector<QVector2D> *data, Polygon *editedP);
	void onAreaRepaint();
	void onPolygonSelect(Polygon *polygon);
	void onEditorReset();
	void onPolygonSave(Polygon *polygon, bool isNew);
	void onEdgeSelect(Edge* edge);
	void onVertexSelect(Vertex* vertex);
	void onPointHighlight(QPointF point);
	void onLineHighlight(QLineF line);
	void onPolygonHighlight(Polygon* poly);
	void onHighlightReset();
	void onEditingExit();
	void onZoom(QPointF oldPos, QPointF newPos);

	void onResetEverything();

	void addNewVertex(QPointF point);
	void editVertexMouse(int row);
	void editVertexCoordMouse(int row, QPointF new_coord);
	void saveVertexMouse(int row);

private:
	Mediator() {}
};

class Area : public QWidget {
	Q_OBJECT
public:
	explicit Area(QWidget *parent = nullptr);

protected:
	void paintEvent(QPaintEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
	void onBufferConnectReceived(QVector<QVector2D> *data, Polygon *editedP);
	void resetHighlight();
	void onPointHighlightReceived(QPointF point);
	void onLineHighlightReceived(QLineF line);
	void onAreaRepaintReceived(); // FIXME: unused?
	void onPolygonHighlightReceived(Polygon* poly);

private:
	// void updateSize(double width, double height);

	/* Helper to find vertex near mouse click
	 * Pass polygon to limit search, or nullptr for all */
	int findVertexAt(const QPoint widgetPos, const Polygon* polygon = nullptr); 

	QVector<QVector2D> *bufferData = nullptr;
	Polygon *editedPolygon = nullptr;
	Polygon *highlightedPolygon = nullptr;
	QPointF pointHighlighted = QPointF(-1, -1);
	QLineF lineHighlighted;

	const int coordOffset = 25; 		// Offset for axis drawing
	double scaleFactor = 1; 				// Current zoom level
	const double scaleStep = 1.15;	// Zoom step
	const int hit_radius_pixels = 5; // Radius for hit testing in *widget pixels*

	int draggingVertex =
		-1;			   // Index of dragged vertex (-1 if no vertex is dragged)
	QPoint dragOffset; // Cursor offset relative to the vertex *in widget pixels*
};

#endif // AREA_H
