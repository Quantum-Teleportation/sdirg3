#include "editor.h"
#include "ui_editor.h"

#include "area.h"
#include "geometry.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QStackedLayout>
#include <QStringList>
#include <QVector>

QVector<QString> materials{"1", "2", "3"};

Editor::Editor(QWidget *parent) : QWidget(parent), ui(new Ui::Editor) {
	// widget setup
	ui->setupUi(this);
	dock = qobject_cast<QDockWidget *>(parentWidget());
	vtable = ui->vertexTable;
	etable = ui->edgeTable;

	// vertexTable setUp
	vtable->setColumnCount(5);
	vtable->setHorizontalHeaderLabels(QStringList()
									  << "Name" << "X" << "Y" << "" << "");
	vtable->horizontalHeader()->setSectionsClickable(false);
	vtable->horizontalHeader()->setSectionsMovable(false);
	vtable->horizontalHeader()->setStyleSheet("QHeaderView::section {"
											  "    border: none;"
											  "}");
	vtable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	// edgeTable setUp
	etable->setColumnCount(2);
	etable->setHorizontalHeaderLabels(QStringList() << "Name" << "Property");
	etable->horizontalHeader()->setSectionsClickable(false);
	etable->horizontalHeader()->setSectionsMovable(false);
	etable->horizontalHeader()->setStyleSheet("QHeaderView::section {"
											  "    border: none;"
											  "}");
	etable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	connect(ui->confirmBtn, &QPushButton::released, this, &Editor::savePolygon);
	connect(ui->cancelBtn, &QPushButton::released, this, &Editor::resetEditor);

	setStyleSheet("QDoubleSpinBox::up-button { width: 0; height: 0; }"
				  "QDoubleSpinBox::down-button { width: 0; height: 0; }"
				  "QDoubleSpinBox { border: 1px solid gray; border-radius: "
				  "4px; padding: 2px 4px; }");

	// vertex table empty row setup
	vtable->insertRow(0);
	QLineEdit *nameEdit = new QLineEdit(this);
	nameEdit->setPlaceholderText("V" + QString::number(polygonNumber) + "_" +
								 QString::number(0));
	// nameEdit->setPlaceholderText("Name");
	vtable->setCellWidget(0, 0, nameEdit);

	QDoubleSpinBox *xEdit = new QDoubleSpinBox(this);
	xEdit->setRange(minCoord, maxCoord);
	xEdit->setDecimals(precision);
	vtable->setCellWidget(0, 1, xEdit);

	QDoubleSpinBox *yEdit = new QDoubleSpinBox(this);
	yEdit->setRange(minCoord, maxCoord);
	yEdit->setDecimals(precision);
	vtable->setCellWidget(0, 2, yEdit);

	QPushButton *addBtn = new QPushButton(this);
	connect(addBtn, &QPushButton::released, this, &Editor::addVertex);
	connect(addBtn, &QPushButton::released, this, &Editor::onBufferConnect);
	vtable->setCellWidget(0, 3, addBtn);
	polygonNumber = Polygon::get_polygons_total();
	addBtn->setIcon(addIcon);

	// opening editor on polygon selection
	connect(Mediator::instance(), &Mediator::onPolygonSelect, this,
			&Editor::onPolygonSelectReceived);
	connect(Mediator::instance(), &Mediator::onEditorReset, this,
			&Editor::resetEditor);

	// testing
	connect(ui->loadBtn, &QPushButton::released, this, &Editor::load);

	// Add Vertext by mouse click
	connect(Mediator::instance(), &Mediator::addNewVertex, this,
			&Editor::addVertexByMouse);

	// Dragging Vertex
	connect(Mediator::instance(), &Mediator::editVertexMouse, this,
			&Editor::editVertex);
	connect(Mediator::instance(), &Mediator::editVertexCoordMouse, this,
			&Editor::editVertexCoord);
	connect(Mediator::instance(), &Mediator::saveVertexMouse, this,
			&Editor::saveVertex);
	connect(Mediator::instance(), &Mediator::saveVertexMouse, this,
			&Editor::onBufferConnect);
}

// Slot for create two testing polygons
void Editor::load() {
	// testing polygon
	// triangle
	addVRow(0, "V0_0", 0.0, 0.0);
	addVRow(1, "V0_1", 700.0, 500.0);
	addVRow(2, "V0_2", 900.0, 100.0);
	savePolygon();
	// square
	addVRow(0, "V1_0", 100.0, 100.0);
	addVRow(1, "V1_1", 100.0, 400.0);
	addVRow(2, "V1_2", 400.0, 400.0);
	addVRow(3, "V1_3", 400.0, 100.0);
	savePolygon();
	ui->loadBtn->hide();
}

// Add Row for Vertex and Edge table
void Editor::addVRow(int row, QString vName, double x, double y) {
	// int row = vtable->rowCount() - 1;

	QLineEdit *nameEdit = new QLineEdit(this);
	nameEdit->insert(vName);
	nameEdit->setPlaceholderText(
		"V" + QString::number(polygonNumber) + "_" +
		QString::number(row)); // Зачем нужна эта строчка, перед ней сразу
							   // заполняется текст?
	nameEdit->setEnabled(false);

	QDoubleSpinBox *xEdit = new QDoubleSpinBox(this);
	xEdit->setRange(minCoord, maxCoord);
	xEdit->setDecimals(precision);
	xEdit->setValue(x);
	xEdit->setEnabled(false);

	QDoubleSpinBox *yEdit = new QDoubleSpinBox(this);
	yEdit->setRange(minCoord, maxCoord);
	yEdit->setDecimals(precision);
	yEdit->setValue(y);
	yEdit->setEnabled(false);

	vtable->insertRow(row);
	vtable->setCellWidget(row, 0, nameEdit);
	vtable->setCellWidget(row, 1, xEdit);
	vtable->setCellWidget(row, 2, yEdit);

	QPushButton *editBtn = new QPushButton(this);
	connect(editBtn, &QPushButton::released, this,
			[this, row]() { editVertex(row); });
	editBtn->setIcon(editIcon);
	QPushButton *saveBtn = new QPushButton(this);
	saveBtn->setIcon(saveIcon);
	connect(saveBtn, &QPushButton::released, this,
			[this, row]() { saveVertex(row); });
	connect(saveBtn, &QPushButton::released, this, &Editor::onBufferConnect);

	QWidget *editSaveStackW = new QWidget(this);
	QStackedLayout *editSaveStack = new QStackedLayout(editSaveStackW);
	editSaveStack->addWidget(editBtn);
	editSaveStack->addWidget(saveBtn);
	vtable->setCellWidget(row, 3, editSaveStackW);

	clearNew();

	// edge table
	etable->insertRow(row);
	QLineEdit *edgeNameEdit = new QLineEdit(this);
	QString defaultEdgeName =
		"E" + QString::number(polygonNumber) + "_" + QString::number(row);
	edgeNameEdit->setPlaceholderText(
		defaultEdgeName); // Зачем нужна эта строчка, дальше идет сразу
						  // заполняется текст?
	edgeNameEdit->setText(defaultEdgeName);
	etable->setCellWidget(row, 0, edgeNameEdit);
	QComboBox *materialCombo = new QComboBox(this);
	materialCombo->addItems(materials);
	etable->setCellWidget(row, 1, materialCombo);

	buffer.append(QVector2D(x, y));
}

// Slot for add Vertex
void Editor::addVertex() {
	int row = vtable->rowCount() - 1;
	// vertex table

	QString name = static_cast<QLineEdit *>(vtable->cellWidget(row, 0))->text();
	if (name.isEmpty()) {
		// return;
		name =
			"V" + QString::number(polygonNumber) + "_" + QString::number(row);
	}

	double x =
		static_cast<QDoubleSpinBox *>(vtable->cellWidget(row, 1))->value();
	double y =
		static_cast<QDoubleSpinBox *>(vtable->cellWidget(row, 2))->value();

	// addVertexRow(row, name, x, y);
	addVRow(row, name, x, y);
}

// Change addMode to editMode
void Editor::editVertex(int row) {
	for (int i = 0; i < 3; ++i) {
		vtable->cellWidget(row, i)->setEnabled(true);
	}

	cancelBtn = new QPushButton(this);
	cancelBtn->setIcon(resetIcon);
	connect(cancelBtn, &QPushButton::released, this,
			[this, row]() { resetVertex(row); });
	vtable->setCellWidget(row, 4, cancelBtn);

	QStackedLayout *layout =
		qobject_cast<QStackedLayout *>(vtable->cellWidget(row, 3)->layout());
	layout->setCurrentIndex(1);

	for (int i = 0; i < vtable->rowCount(); ++i) {
		if (i != row) {
			vtable->cellWidget(i, 3)->setEnabled(false);
		}
	}
}

void Editor::saveVertex(int row) {
	finishEditVertex(row);

	double x =
		qobject_cast<QDoubleSpinBox *>(vtable->cellWidget(row, 1))->value();
	double y =
		qobject_cast<QDoubleSpinBox *>(vtable->cellWidget(row, 2))->value();

	buffer[row] = QVector2D(x, y);
}

void Editor::resetVertex(int row) {
	finishEditVertex(row);

	QDoubleSpinBox *xEdit =
		qobject_cast<QDoubleSpinBox *>(vtable->cellWidget(row, 1));
	xEdit->setValue(buffer[row][0]);
	QDoubleSpinBox *yEdit =
		qobject_cast<QDoubleSpinBox *>(vtable->cellWidget(row, 2));
	yEdit->setValue(buffer[row][1]);
}

void Editor::finishEditVertex(int row) {
	for (int i = 0; i < 3; ++i) {
		vtable->cellWidget(row, i)->setEnabled(false);
	}

	vtable->removeCellWidget(row, 4);
	delete cancelBtn;
	cancelBtn = nullptr;
	QStackedLayout *layout =
		qobject_cast<QStackedLayout *>(vtable->cellWidget(row, 3)->layout());
	layout->setCurrentIndex(0);
	for (int i = 0; i < vtable->rowCount(); ++i) {
		vtable->cellWidget(i, 3)->setEnabled(true);
	}
}

void Editor::clearNew() {
	int row = vtable->rowCount() - 1;
	QLineEdit *nameEdit = static_cast<QLineEdit *>(vtable->cellWidget(row, 0));
	nameEdit->setText("");
	nameEdit->setPlaceholderText("V" + QString::number(polygonNumber) + "_" +
								 QString::number(row));
	static_cast<QDoubleSpinBox *>(vtable->cellWidget(row, 1))->setValue(0);
	static_cast<QDoubleSpinBox *>(vtable->cellWidget(row, 2))->setValue(0);
}

void Editor::updateTableSize() {
	// vertex table
	auto tableWidth = vtable->width();
	for (auto col = 0; col < 3; ++col) {
		vtable->setColumnWidth(col, tableWidth * 0.3);
	}
	for (auto col = 3; col < 6; ++col) {
		vtable->setColumnWidth(col, tableWidth * 0.05);
	}

	// edge table
	tableWidth = etable->width();
	etable->setColumnWidth(0, tableWidth * 0.5);
	etable->setColumnWidth(1, tableWidth * 0.5);
}

void Editor::showEvent([[maybe_unused]] QShowEvent *event) {
	updateTableSize();
}

void Editor::resizeEvent([[maybe_unused]] QResizeEvent *event) {
	updateTableSize();
}

void Editor::onBufferConnect() {
	// emit Mediator::instance()->bufferConnect(&buffer, editedPolygon);
	// qWarning() << "sent";
	emit Mediator::instance() -> onBufferConnect(&buffer, editedPolygon);
}

void Editor::savePolygon() {
	if (etable->rowCount() < 3) {
		return;
	}
	QString name = "P" + QString::number(polygonNumber);
	QUuid id;
	if (editedPolygon != nullptr) {
		name = editedPolygon->name();
		id = editedPolygon->id();
		editedPolygon->delete_polygon();
		editedPolygon = nullptr;
	}

	QVector<QUuid> vertices;
	QVector<QUuid> edges;

	// создаём точки
	for (int row = 0; row < etable->rowCount(); ++row) {
		QString vname =
			qobject_cast<QLineEdit *>(vtable->cellWidget(row, 0))->text();
		double x =
			qobject_cast<QDoubleSpinBox *>(vtable->cellWidget(row, 1))->value();
		double y =
			qobject_cast<QDoubleSpinBox *>(vtable->cellWidget(row, 2))->value();
		Vertex v(x, y, vname);
		vertices.append(v.id());
	}

	// создаём рёбра
	for (int row = 0; row < etable->rowCount(); ++row) {
		QString ename =
			qobject_cast<QLineEdit *>(etable->cellWidget(row, 0))->text();
		if (ename == "") {
			ename = "E" + QString::number(polygonNumber) + "_" +
					QString::number(row);
		}
		int property = qobject_cast<QComboBox *>(etable->cellWidget(row, 1))
						   ->currentText()
						   .toInt();
		Edge e(vertices[row], vertices[(row + 1) % etable->rowCount()], ename,
			   property);
		edges.append(e.id());
	}

	// где-то здесь должна быть проверка корректности

	// создаём полигон
	Polygon *p = new Polygon(vertices, edges, name, 1, polygonNumber, id);

	// emit Mediator::instance()->polygonAdd(&p);
	// if (!id.isNull()) {
	//     mainWindow->removePolygon(id);
	// }
	// mainWindow->addPolygon(p);
	emit Mediator::instance() -> onPolygonSave(p, id.isNull());
	resetEditor();
}

void Editor::resetEditor() {
	while (vtable->rowCount() > 1) {
		vtable->removeRow(0);
	}
	while (etable->rowCount() > 0) {
		etable->removeRow(0);
	}

	buffer.clear();
	emit Mediator::instance() -> onBufferConnect(&buffer, nullptr);
	polygonNumber = Polygon::get_polygons_total();
	clearNew();
	editedPolygon = nullptr;
	dock->close();
}

void Editor::onPolygonSelectReceived(Polygon *polygon) {
	// dock->show();
	if (polygon == editedPolygon) {
		dock->show();
		emit Mediator::instance() -> onBufferConnect(&buffer, polygon);
		return;
	}
	resetEditor();
	setupExistingPolygon(polygon);
}

void Editor::setupExistingPolygon(Polygon *polygon) {
	editedPolygon = polygon;
	polygonNumber = editedPolygon->number();
	// emit Mediator::instance()->onBufferConnect(&buffer, polygon);
	for (auto &vId : polygon->vertices) {
		Vertex &v = all_vertices[vId];
		QString vName = v.name();
		double x = v.x();
		double y = v.y();
		// addVertexRow(vtable->rowCount() - 1, vName, x, y);
		addVRow(vtable->rowCount() - 1, vName, x, y);
	}

	for (auto &eId : polygon->edges) {
		Edge &e = all_edges[eId];
		QString eName = e.name();
		QString material = QString::number(e.get_property());

		qobject_cast<QLineEdit *>(etable->cellWidget(etable->rowCount() - 1, 0))
			->setText(eName);
		qobject_cast<QComboBox *>(etable->cellWidget(etable->rowCount() - 1, 1))
			->setCurrentText(material);
		dock->show();
		emit Mediator::instance() -> onBufferConnect(&buffer, polygon);
	}
}

// Add Vertext by mouse click
void Editor::addVertexByMouse(QPointF point) {
	int row = vtable->rowCount() - 1;
	QString name =
		"V" + QString::number(polygonNumber) + "_" + QString::number(row);

	// For check open dock or not
	if (dock->isVisible()) {
		addVRow(row, name, point.x(), point.y());
		onBufferConnect();
	} else {
		resetEditor();
		dock->show();
		addVRow(row, name, point.x(), point.y());
	}
}

// Change coord for vertex when move by mouse
void Editor::editVertexCoord(int row, QPointF new_coord) {
	qobject_cast<QDoubleSpinBox *>(vtable->cellWidget(row, 1))
		->setValue(new_coord.x());
	qobject_cast<QDoubleSpinBox *>(vtable->cellWidget(row, 2))
		->setValue(new_coord.y());

	buffer[row] = QVector2D(new_coord.x(), new_coord.y());
	onBufferConnect();
}

Editor::~Editor() { delete ui; }
