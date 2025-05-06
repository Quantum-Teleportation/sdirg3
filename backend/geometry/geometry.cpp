#include "geometry.h"

#include <QUuid>

namespace Geometry {

bool is_segment_intersected(const QPointF &a1, const QPointF &a2,
							const QPointF &b1, const QPointF &b2) noexcept {
	return is_projection_intersected(a1.x(), a2.x(), b1.x(), b2.x()) &&
		   is_projection_intersected(a1.y(), a2.y(), b1.y(), b2.y()) &&
		   orientated_area(a1, a2, b1) * orientated_area(a1, a2, b2) <= 0 &&
		   orientated_area(b1, b2, a1) * orientated_area(b1, b2, a2) <= 0;
}

bool point_in_polygon(const QPointF &p, const Polygon &polygon) {
	QPointF ray(922337203685477632, 0);
	// TODO: what if this rand ray is equal to polygon vertex???
	std::size_t crossings = 0;
	for (auto &edge : polygon.edges) {
		auto e = all_edges.find(edge).value();
		if (is_segment_intersected(
				p, ray, all_vertices.find(e.coords().first).value(),
				all_vertices.find(e.coords().second).value())) {
			++crossings;
		}
	}
	return crossings & 1;
}

QVector<Polygon *> find_polygons_by_point(const QPointF &p) {
	QVector<Polygon *> polygons;

	// mb std::for_each here better?
	for (auto &polygon : all_polygons) {
		if (point_in_polygon(p, polygon)) {
			polygons.append(&polygon);
		}
	}
	return polygons;
}

bool isConvex(const QVector<QPointF> &vertices) {
	double init = cross(
		{vertices[1].x() - vertices[0].x(), vertices[1].y() - vertices[0].y()},
		{vertices[2].x() - vertices[1].x(), vertices[2].y() - vertices[1].y()});

	for (int i = 1; i < vertices.size(); i++) {
		double current_cross =
			cross({vertices[i].x() - vertices[i - 1].x(),
				   vertices[i].y() - vertices[i - 1].y()},
				  {vertices[(i + 1) % vertices.size()].x() - vertices[i].x(),
				   vertices[(i + 1) % vertices.size()].y() - vertices[i].y()});
		if (init * current_cross < 0) {
			return false;
		}
	}
	return true;
}

bool isNotIntersecting(const QVector<QPointF> &vertices) {
	for (int i = 0; i < vertices.size(); i++) {
		for (const auto &pair : all_edges) {
			if (is_segment_intersected(all_vertices[pair.coords().first],
									   all_vertices[pair.coords().second],
									   vertices[i],
									   vertices[(i + 1) % vertices.size()])) {
				return false;
			}
		}
	}
	return true;
}

} // namespace Geometry
