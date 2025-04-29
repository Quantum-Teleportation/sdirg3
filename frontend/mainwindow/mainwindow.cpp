#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "editor.h"
#include "generator.h"

#include <QPalette>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);

	ui->toolBar->addAction(ui->actionSave);
	QObject::connect(ui->actionSave, &QAction::triggered, this, &MainWindow::saveConfigs);


	ui->editorDock->setWidget(new Editor(ui->editorDock)); // ??
	connect(ui->editorDock, &QDockWidget::visibilityChanged, this, [this]() {
		if (!ui->editorDock->isVisible()) {
			emit Mediator::instance() -> onEditorReset();
		}
	});
	ui->editorDock->close();

	ui->tree->setHeaderHidden(true);
	connect(ui->newPolygonBtn, &QPushButton::released, this,
			&MainWindow::newPolygon);
	// connect(ui->tree, &QTreeWidget::itemClicked, this, onIte);
	connect(ui->tree, &QTreeWidget::itemDoubleClicked,
			[this](QTreeWidgetItem *item, [[maybe_unused]] int column) {
				QUuid id = item->data(0, Qt::UserRole).value<QUuid>();
				selectPolygon(id);
			});
	connect(Mediator::instance(), &Mediator::onPolygonSave, this,
			[this](Polygon *polygon, bool isNew) {
				if (!isNew) {
					removePolygon(polygon->id());
				}
				addPolygon(polygon);
			});
	ui->tree->setSortingEnabled(true);
	ui->tree->sortByColumn(0, Qt::AscendingOrder);

	setWindowTitle("Grid Editor");
	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);

	QColor gray(229, 228, 226);
	QPalette pal = QPalette();
	pal.setColor(QPalette::Window, gray);

	ui->editorDock->setAutoFillBackground(true);
	ui->editorDock->setPalette(pal);
	ui->treeDock->setAutoFillBackground(true);
	ui->treeDock->setPalette(pal);

	setStyleSheet("QMainWindow::separator{ width: 0px; height: 0px; }");
	// setDockOptions(QMainWindow::GroupedDockWidgets);

	// Exit by Ctrl+Q
	auto actionClose = new QAction();
	actionClose->setShortcut(QKeySequence::Quit);
	addAction(actionClose);
	QObject::connect(actionClose, &QAction::triggered, this,
					 &QCoreApplication::quit);
}

void MainWindow::newPolygon() {
	emit Mediator::instance() -> onEditorReset();
	ui->editorDock->show();
}

void MainWindow::addPolygon(Polygon *polygon) {
	QTreeWidgetItem *polyItem = new QTreeWidgetItem(ui->tree);
	polyItem->setText(0, polygon->name());
	polyItem->setData(0, Qt::UserRole, QVariant::fromValue(polygon->id()));

	QTreeWidgetItem *vertexFolder = new QTreeWidgetItem(polyItem);
	vertexFolder->setText(0, "Vertices");
	QTreeWidgetItem *edgeFolder = new QTreeWidgetItem(polyItem);
	edgeFolder->setText(0, "Edges");

	for (auto &vertex : polygon->vertices) {
		QTreeWidgetItem *v = new QTreeWidgetItem(vertexFolder);
		v->setText(0, all_vertices[vertex].name());
	}
	for (auto &edge : polygon->edges) {
		QTreeWidgetItem *e = new QTreeWidgetItem(edgeFolder);
		e->setText(0, all_edges[edge].name());
	}
}

void MainWindow::removePolygon(QUuid id) {
	QVariant target = QVariant::fromValue(id);
	int count = ui->tree->topLevelItemCount();
	// Проходим по всем дочерним элементам первого уровня
	for (int i = 0; i < count; ++i) {
		QTreeWidgetItem *item =
			ui->tree->takeTopLevelItem(0); // Убираем первый элемент
		if (item->data(0, Qt::UserRole) == target) {
			delete item; // Удаляем поддерево
			return;		 // После удаления выходим
		}
	}
}

void MainWindow::selectPolygon(QUuid id) {
	if (id.isNull()) {
		return;
	}
	Polygon *polygon = &all_polygons[id];
	ui->editorDock->show();
	emit Mediator::instance() -> onPolygonSelect(polygon);
}

void MainWindow::saveConfigs() {
	qDebug() << "Emit Save configs";
	terraformer::generator gen;
	gen.saveAs("test.conf");
}

MainWindow::~MainWindow() { delete ui; }
