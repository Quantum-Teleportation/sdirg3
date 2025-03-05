#include "area.h"
#include "geometry.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QPointF>
#include <QVector2D>

Area::Area(QWidget *parent) : QWidget{parent} {
	// connect(Mediator::instance(), &Mediator::bufferConnect, this,
	// &Area::onBufferConnectReceived); qWarning() << "constr";
	connect(Mediator::instance(), &Mediator::onBufferConnect, this,
			&Area::onBufferConnectReceived);

	setMouseTracking(true); // For check mouse position
}

void Area::paintEvent([[maybe_unused]] QPaintEvent *event) {
	// qWarning() << "paint";
	QPainter painter(this);
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);

	QBrush brush(Qt::black, Qt::SolidPattern);
	QPen pen(Qt::black);

	// Настройка кисти для рисования точек
	painter.setBrush(Qt::black);
	painter.setPen(Qt::black);
	// for (auto& [id, polygon] : all_polygons) {
	//     if (edited == &polygon) {
	//         continue;
	//     }
	//     QVector<QUuid>& vertices = polygon.get_vertices();
	//     qWarning() << vertices.size();
	//     for (int i = 0; i < vertices.size(); ++i) {
	//         Vertex& curV = all_vertices[vertices[i]];
	//         Vertex& nextV = all_vertices[vertices[(i + 1) %
	//         vertices.size()]];

	//         painter.drawEllipse(QPointF(curV.get_x(),
	//         curV.get_y()), 3.0, 3.0); painter.drawLine(QPointF(curV.get_x(),
	//         curV.get_y()), QPointF(nextV.get_x(), nextV.get_y()));
	//     }
	// }

	for (auto polygons = all_polygons.begin(); polygons != all_polygons.end();
		 ++polygons) {
		Polygon &polygon = polygons.value();
		if (edited == &polygon) {
			continue;
		}
		QVector<QUuid> &vertices = polygon.vertices;
		qDebug() << vertices.size();
		for (int i = 0; i < vertices.size(); ++i) {
			Vertex &curV = all_vertices[vertices[i]];
			Vertex &nextV = all_vertices[vertices[(i + 1) % vertices.size()]];

			painter.drawEllipse(QPointF(curV.x(), curV.y()), 3.0, 3.0);
			painter.drawLine(QPointF(curV.x(), curV.y()),
							 QPointF(nextV.x(), nextV.y()));
		}
	}
	if (bufferData == nullptr || bufferData->size() == 0) {
		return;
	}

	// brush.setColor(Qt::red);
	painter.setBrush(Qt::red);
	painter.setPen(Qt::red);

	// for (const QVector2D &point : *bufferData) {
	for (int i = 0; i < bufferData->size(); ++i) {
		QVector2D point = bufferData[0][i];
		QVector2D nextPoint;
		if (i < bufferData->size() - 1) {
			nextPoint = bufferData[0][i + 1];
		} else {
			nextPoint = bufferData[0][0];
		}
		painter.drawEllipse(QPointF(point.x(), point.y()), 3.0, 3.0);
		painter.drawLine(QPointF(point.x(), point.y()),
						 QPointF(nextPoint.x(), nextPoint.y()));
	}
	painter.end();
}

void Area::mousePressEvent(QMouseEvent *event) {
	if (event->button() == Qt::LeftButton) {
		auto pos = mapFromGlobal(event->globalPosition().toPoint());
		auto candidates = Geometry::find_polygons_by_point(pos);
		if (candidates.size() == 0) {
			emit Mediator::instance()
				-> addNewVertex(&pos); // If we have not selected any polygon,
									   // we create a new point.
			return;
		}

		auto &last_candidate = all_polygons[candidates.last()];
		auto vertices = last_candidate.vertices;

		// emit
		// Mediator::instance()->polygonSelect(&all_polygons[candidates.back()]);
		emit Mediator::instance()
			-> onPolygonSelect(&last_candidate); // FIXME: Если заменить на
												 // last_coniddate, то ломается

		// Verification of click on the vertex
		for (int i = 0; i < vertices.size(); ++i) {
			QPoint cur_point_vertex{int(all_vertices[vertices.at(i)].x()),
									int(all_vertices[vertices.at(i)].y())};
			if (QRect(cur_point_vertex - QPoint(3, 3), QSize(10, 10))
					.contains(event->pos())) {
				qDebug() << "Click on point with coord: " << cur_point_vertex;
				draggingVertex = i;
				dragOffset = event->pos() - cur_point_vertex;
				emit Mediator::instance() -> editVertexMouse(draggingVertex);
				break;
			}
		}
	}
}

// В теории тут будем ещё отслеживать мышку, для изменения курсора при наведении
// на точку
void Area::mouseMoveEvent(QMouseEvent *event) {
	if (draggingVertex != -1) {
		auto new_coord = event->pos() - dragOffset;
		emit Mediator::instance() -> editVertexCoordMouse(draggingVertex,
														  &new_coord);
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

void Area::onBufferConnectReceived(QVector<QVector2D> *data, Polygon *editedP) {
	if (bufferData == nullptr) {
		bufferData = data;
	}
	edited = editedP;
	repaint();
	// qWarning() << "signal";
}
