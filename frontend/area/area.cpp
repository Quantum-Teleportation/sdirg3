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
}

void Area::paintEvent(QPaintEvent *event) {
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

    for (auto polygons =  all_polygons.begin(); polygons !=  all_polygons.end(); ++polygons) {
        QUuid id = polygons.key();
        Polygon& polygon = polygons.value();
        if (edited == &polygon) {
            continue;
        }
        QVector<QUuid>& vertices = polygon.vertices;
        qWarning() << vertices.size();
        for (int i = 0; i < vertices.size(); ++i) {
            Vertex& curV = all_vertices[vertices[i]];
            Vertex& nextV = all_vertices[vertices[(i + 1) % vertices.size()]];

            painter.drawEllipse(QPointF(curV.x(), curV.y()), 3.0, 3.0);
            painter.drawLine(QPointF(curV.x(), curV.y()), QPointF(nextV.x(), nextV.y()));
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
	auto pos = mapFromGlobal(event->globalPosition().toPoint());
	auto candidates = find_polygons_by_point(pos.x(), pos.y());
	if (candidates.size() == 0) {
		return;
	}
	// emit
	// Mediator::instance()->polygonSelect(&all_polygons[candidates.back()]);
	emit Mediator::instance() -> onPolygonSelect(
								  &all_polygons[candidates.back()]);
}

void Area::onBufferConnectReceived(QVector<QVector2D> *data, Polygon *editedP) {
	if (bufferData == nullptr) {
		bufferData = data;
	}
	edited = editedP;
	repaint();
	// qWarning() << "signal";
}
