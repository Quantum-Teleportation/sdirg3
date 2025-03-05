#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QHash>
#include <QPair>
#include <QPointF>
#include <QString>
#include <QUuid>
#include <QVector>
#include <map>

#include "edge.h"
#include "polygon.h"
#include "vertex.h"

namespace Geometry {

/*
 * orientated area calculated by vector multiplication
 * args are points ( not vectors )
 */
inline double orientated_area(const QPointF &a, const QPointF &b,
							  const QPointF &c) noexcept {
	return (b.x() - a.x()) * (c.y() - a.y()) -
		   (b.y() - a.y()) * (c.x() - a.x());
}

/*
 * check: is projection on axis are intersected
 * (for segments (a1, a2), (b1, b2))
 * fastcall x64 get 4 max args (so let be it)
 * min and max has maxsd minxsd xmm operations (mb std::minmax???)
 * use bit AND (not logical) for xmm
 */
inline bool is_projection_intersected(double a1, double a2, double b1,
									  double b2) noexcept {
	return int(std::max(a1, a2) >= std::min(b1, b2)) &
		   int(std::max(b1, b2) >= std::min(a1, a2));
}

/*
 *	check: is two segments are intersected
 */
bool is_segment_intersected(const QPointF &a1, const QPointF &a2,
							const QPointF &b1, const QPointF &b2) noexcept;

/*
 * check : is is point in polygon
 */
bool point_in_polygon(const QPointF &p, QUuid polygon_id);

/*
 * Annotation: in actual version of drig3 project this function are commented
 * and I realy do not know why this function is here...
 */
// bool check_new_point(const QPointF &a, const QPointF &b, const QPointF &c);

/*
 *	Find polygon uuid by point
 */
QVector<QUuid> find_polygons_by_point(const QPointF &p);

/*
 *	Vectors multiplication
 *	(args are vectors)
 */
inline double cross(const QPointF &a, const QPointF &b) noexcept {
	return a.x() * b.y() - b.x() * a.y();
}

/*
 * check : is third point from one side with others two for checknig convex algo
 */
inline bool check_convex(const QPointF &a, const QPointF &b,
						 const QPointF &c) noexcept {
	return (cross(b - a, c - b) <= 0);
}

/*
 * check: is vertices form a convex polygon
 */
bool isConvex(const QVector<QPointF> &vertices);

/*
 * check: is vertices form a non self-intersected polygon
 */
bool isNotIntersecting(const QVector<QPointF> &vertices);

/*
 *	check: polygon for convex and not self-intersected(as I can understand)
 */
inline bool checkPolygon(const QVector<QPointF> &vertices) {
	return int(isConvex(vertices)) & int(isNotIntersecting(vertices));
}

} // namespace Geometry

#endif
