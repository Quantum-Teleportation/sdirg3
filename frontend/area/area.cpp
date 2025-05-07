#include "area.h"
#include "geometry.h"

#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QPointF>
#include <QVector2D>
#include <QPolygonF>
#include <QPainter>
#include <QScrollArea>
#include <QScrollBar>

Area::Area(QWidget *parent) : QWidget{parent} {
	connect(Mediator::instance(), &Mediator::onBufferConnect, this,
			&Area::onBufferConnectReceived);
	connect(Mediator::instance(), &Mediator::onHighlightReset, this,
			&Area::resetHighlight);
	connect(Mediator::instance(), &Mediator::onPointHighlight, this,
			&Area::onPointHighlightReceived);
	connect(Mediator::instance(), &Mediator::onLineHighlight, this,
			&Area::onLineHighlightReceived);
	connect(Mediator::instance(), &Mediator::onPolygonHighlight, this,
			&Area::onPolygonHighlightReceived);

	setMouseTracking(true); // For check mouse position

	setMinimumSize(QSize(1000 + 2 * coordOffset, 1000 + 2 * coordOffset)); // TODO: think about default size 
}

void Area::paintEvent([[maybe_unused]] QPaintEvent *event) {
	QPainter painter(this);
	if(!painter.isActive()) {
		painter.begin(this);
	}
	painter.translate(coordOffset, coordOffset);
	painter.scale(scaleFactor, scaleFactor);

	painter.setRenderHint(QPainter::Antialiasing);

	// рамка и оси
	QPen axisPen;
	axisPen.setColor(Qt::gray);
	axisPen.setWidth(1 / scaleFactor);
	painter.setPen(axisPen);
	painter.drawLine(QPointF(0, 0), QPointF(width() / scaleFactor + coordOffset / scaleFactor, 0));
	painter.drawLine(QPointF(0, 0), QPointF(0, height() / scaleFactor + coordOffset / scaleFactor));

	// точки на осях
	QFont axisFont;
	axisFont.setPointSize(8 / scaleFactor);
	painter.setFont(axisFont);
	int tickStep = 100;
	double tickHalfSize = 5 / scaleFactor;
	for (int curTick = 0; curTick < (width() - coordOffset) / scaleFactor; curTick += tickStep) {
		if (curTick == 0) continue; // Avoid drawing 0 tick twice
		painter.drawLine(curTick, -tickHalfSize, curTick, tickHalfSize);
		painter.drawText(curTick, -tickHalfSize, QString::number(curTick));
	}
	for (int curTick = tickStep; curTick < (height() - coordOffset) / scaleFactor; curTick += tickStep) {
		if (curTick == 0) continue; // Avoid drawing 0 tick twice
		painter.drawLine(-tickHalfSize, curTick, tickHalfSize, curTick);
		painter.drawText(-coordOffset, curTick + tickHalfSize, QString::number(curTick));
	}

	QPen normalPen;
	normalPen.setWidth(2.0 / scaleFactor);
	normalPen.setColor(Qt::black);
	painter.setPen(normalPen);

	// полигоны 
	/* FIXME: Плохо, что мы получаем полигоны из глобальной переменной.  
	 * Эти данные должен предоставлять слой контроллера/сервиса. */
	for (auto polygons = all_polygons.begin(); polygons != all_polygons.end();
		 ++polygons) {
		Polygon &polygon = polygons.value();
		if (editedPolygon == &polygon) {
			continue;
		}
		QVector<QUuid> &vertices = polygon.vertices;
		for (int i = 0; i < vertices.size(); ++i) {
			Vertex &curV = all_vertices[vertices[i]];
			Vertex &nextV = all_vertices[vertices[(i + 1) % vertices.size()]];

			painter.drawEllipse(QPointF(curV.x(), curV.y()), 3.0 / scaleFactor, 3.0 / scaleFactor);
			painter.drawLine(QPointF(curV.x(), curV.y()),
							 QPointF(nextV.x(), nextV.y()));
		}
	}
	if (bufferData == nullptr || bufferData->size() == 0) {
		return;
	}

	// редактируемый полигон
	if (!(bufferData == nullptr || bufferData->size() == 0)) {
		QPen editedPen;
		editedPen.setWidth(4.0 / scaleFactor);
		editedPen.setColor(Qt::red);
		painter.setPen(editedPen);

		for (int i = 0; i < bufferData->size(); ++i) {
			QVector2D point = bufferData[0][i];
			QVector2D nextPoint;
			if (i < bufferData->size() - 1) {
				nextPoint = bufferData[0][i + 1];
			} else {
				nextPoint = bufferData[0][0];
			}
			painter.drawEllipse(QPointF(point.x(), point.y()), 3.0 / scaleFactor, 3.0 / scaleFactor);
			painter.drawLine(QPointF(point.x(), point.y()),
							QPointF(nextPoint.x(), nextPoint.y()));
		}
	}

	// выделенный в дереве
	QPen highlightPen;
	highlightPen.setColor(Qt::blue);
	highlightPen.setWidth(4.0 / scaleFactor);
	painter.setPen(highlightPen);

	/* FIXME: Плохо, что мы получаем полигоны из глобальной переменной.  
	 * Эти данные должен предоставлять слой контроллера/сервиса. */
	if (highlightedPolygon != nullptr) {
		for (auto& edgeId : highlightedPolygon->edges) {
			Edge& edge = all_edges[edgeId];
			Vertex& v1 = all_vertices[edge.coords().first];
			Vertex& v2 = all_vertices[edge.coords().second];
			QLineF line({v1.x(), v1.y()}, {v2.x(), v2.y()});
			painter.drawLine(line);
		}
	}
	else if (pointHighlighted.x() != -1) {
		QBrush vertexBrush;
		vertexBrush.setStyle(Qt::SolidPattern);
		vertexBrush.setColor(Qt::blue);
		painter.setBrush(vertexBrush);
		painter.drawEllipse(pointHighlighted, 5.0 / scaleFactor, 5.0 / scaleFactor);
	} else if (lineHighlighted.length()) {
		painter.drawLine(lineHighlighted);
	}

	painter.end();
}

// Helper function to find vertex near mouse position (in widget pixels)
// Returns index in editedPolygon->vertices or -1 if none found
int Area::findVertexAt(const QPoint widgetPos, const Polygon* polygon) {
	const Polygon* targetPolygon = polygon ? polygon : editedPolygon;

	if (!targetPolygon || targetPolygon->vertices.isEmpty()) return -1;

	const QVector<QUuid>& vertices = targetPolygon->vertices;

	for (int i = 0; i < vertices.size(); ++i) {
		const Vertex& v = all_vertices[vertices.at(i)];
		QPointF backendVertexPos(v.x(), v.y());

		// Convert backend position to the position where it's DRAWN in widget pixels
		QPointF drawnWidgetPos = (backendVertexPos + QPointF(coordOffset / scaleFactor, coordOffset / scaleFactor)) * scaleFactor;

		// Create a hit test rectangle around the drawn position in widget pixels
		QRectF hitRect(drawnWidgetPos.x() - hit_radius_pixels, drawnWidgetPos.y() - hit_radius_pixels,
										2 * hit_radius_pixels, 2 * hit_radius_pixels);

		if (hitRect.contains(widgetPos)) {
			// qDebug() << "Hit vertex" << i << "at backend coord:" << backendVertexPos << "drawn at:" << drawnWidgetPos; 
			return i;
		}
	}
	return -1;
}

void Area::mousePressEvent(QMouseEvent *event) {
	if (event->button() == Qt::LeftButton) {
		QPoint widgetPos = event->pos();
		int hitIndex = findVertexAt(widgetPos, editedPolygon);

		
		if(hitIndex != -1) {  // Drag start
			draggingVertex = hitIndex;
			// Get the backend position of the grabbed vertex
			const Vertex& v = all_vertices[editedPolygon->vertices.at(draggingVertex)];
			QPointF backendVertexPos(v.x(), v.y());
			QPointF drawnWidgetPos = (backendVertexPos + QPointF(coordOffset, coordOffset)) * scaleFactor;
			dragOffset = widgetPos - drawnWidgetPos.toPoint();
			emit Mediator::instance()->editVertexMouse(draggingVertex);
			return; // Vertex was grabbed, don't proceed to polygon selection or adding
		}

		// If no vertex was grabbed, check for polygon selection
		QPointF widgetPosF(widgetPos);
		QPointF backendPos = widgetPosF / scaleFactor - QPointF(coordOffset / scaleFactor, coordOffset/ scaleFactor);
		auto candidates = Geometry::find_polygons_by_point(backendPos);

		if (candidates.size() == 0) {
            // If no polygon was selected, add a new vertex
			emit Mediator::instance()->addNewVertex(backendPos);
			return; // Vertex added, don't proceed
		}

		// If a polygon was selected, signal it
		auto &last_candidate = *(candidates.last());
		emit Mediator::instance()->onPolygonSelect(&last_candidate); 
	} else if ((event->button() == Qt::RightButton)) {
		// Debug: Print all vertex coordinates
		for (auto iter = all_vertices.begin(); iter != all_vertices.end(); ++iter) {
			qDebug() << "Backend Vertex:" << QPointF(iter->x(), iter->y());
		}
		// Debug: Print edited polygon buffer coordinates
		if (bufferData) {
			qDebug() << "Editor Buffer (Backend Coords):";
			for(const auto& p : *bufferData) {
				qDebug() << QPointF(p.x(), p.y());
			}
		}
	}
}

void Area::mouseMoveEvent(QMouseEvent *event) {
	if (draggingVertex != -1) {
		QPoint widgetPos = event->pos();
		QPointF newDrawnWidgetPos = widgetPos - dragOffset;
		QPointF newBackendPos = newDrawnWidgetPos / scaleFactor - QPointF(coordOffset, coordOffset);

		// clang-format off
		emit Mediator::instance() -> editVertexCoordMouse(draggingVertex, newBackendPos);
		// clang-format on
		repaint();
	} else {
		bool nearVertex = findVertexAt(event->pos(), editedPolygon) != -1;
		if (!nearVertex) {
			nearVertex = findVertexAt(event->pos(), nullptr) != -1; // Check all polygons
		}
		if (nearVertex) {
			setCursor(Qt::PointingHandCursor);
		} else {
			unsetCursor();
		}
	}
}

void Area::mouseReleaseEvent(QMouseEvent *event) {
	if (event->button() == Qt::LeftButton) {
		if (draggingVertex != -1) {
			emit Mediator::instance() -> saveVertexMouse(draggingVertex);
		}
		draggingVertex = -1;
	}
}

void Area::wheelEvent(QWheelEvent *event) {
	QPoint numDegrees = event->angleDelta() / 8;

	double deltaY = numDegrees.y();
	double currentFactor = 1.0;

	if (deltaY > 0) {
		currentFactor = scaleStep;
	} else if (deltaY < 0){
		currentFactor = 1.0 / scaleStep;
	} else {
		return;
	}

	// Get the position in widget pixels where the mouse is centered
	QPointF widgetPos = event->position();
	// Convert this widget position to the *backend* coordinate that is under the mouse
	QPointF backendPosBefore = widgetPos / scaleFactor - QPointF(coordOffset, coordOffset);
	scaleFactor *= currentFactor;
	// TODO: think about minimum and maximum zoom
	if (scaleFactor < 1.0) scaleFactor = 1.0;	// Minimum zoom out
	if (scaleFactor > 3.0) scaleFactor = 3.0;	// Maximum zoom in 

	// Calculate the new drawn position of the *same backend coordinate* after scaling
	QPointF newDrawnWidgetPos = (backendPosBefore + QPointF(coordOffset, coordOffset)) * scaleFactor;

	// Calculate the difference between the mouse position (widgetPos) and the new drawn position
	// This difference is the translation needed to keep the point under the mouse
	QPointF translationNeeded = widgetPos - newDrawnWidgetPos;

	if (QScrollArea *parentScrollArea = qobject_cast<QScrollArea *>(parentWidget())) {
		QScrollBar *hBar = parentScrollArea->horizontalScrollBar();
		QScrollBar *vBar = parentScrollArea->verticalScrollBar();
		if (hBar && vBar) {
			hBar->setValue(hBar->value() - translationNeeded.x());
			vBar->setValue(vBar->value() - translationNeeded.y());
		}
	}

	double contentWidth  = 1000.0;
	double contentHeight = 1000.0;
	setMinimumSize(QSize((contentWidth + coordOffset) * scaleFactor + coordOffset,
											 (contentHeight + coordOffset) * scaleFactor + coordOffset));

	repaint();
}

void Area::onBufferConnectReceived(QVector<QVector2D> *data, Polygon *editedP) {
	if (bufferData == nullptr) {
		bufferData = data;
	}
	editedPolygon = editedP;
	repaint();
}

void Area::resetHighlight() {
	highlightedPolygon = nullptr;
	pointHighlighted.setX(-1);
	pointHighlighted.setY(-1);
	lineHighlighted.setP1(QPointF(0, 0));
	lineHighlighted.setP2(QPointF(0, 0));
	repaint();
}

void Area::onPolygonHighlightReceived(Polygon* polygon) {
	resetHighlight();
	highlightedPolygon = polygon;
	repaint();
}

void Area::onPointHighlightReceived(QPointF point) {
	resetHighlight();
	pointHighlighted = point;
	repaint();
}

void Area::onLineHighlightReceived(QLineF line) {
	resetHighlight();
	lineHighlighted = line;
	repaint();
}

void Area::onAreaRepaintReceived() {
	repaint();
}