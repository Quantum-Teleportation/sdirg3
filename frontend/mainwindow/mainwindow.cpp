#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "editor.h"
#include "generator.h"

#include <QPalette>
#include <fstream>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);

	ui->editorDock->setWidget(new Editor(ui->editorDock)); // ??
	connect(ui->editorDock, &QDockWidget::visibilityChanged, this, [this]() {
		if (!ui->editorDock->isVisible()) {
			emit Mediator::instance() -> onEditorReset();
		}
	});
	ui->editorDock->close();

	// --- New Generator Configuration Dock Setup ---
	generatorConfigWidget = new GeneratorConfigWidget(ui->generatorDockWidget);
	ui->generatorDockWidget->setWidget(generatorConfigWidget);
	ui->generatorDockWidget->hide();
	ui->toolBar->addAction(ui->actionConfig);
	QObject::connect(ui->actionConfig, &QAction::triggered, this,
					 &MainWindow::toggleGeneratorDock);
	QObject::connect(generatorConfigWidget, SIGNAL(create_material_files()),
					 this, SLOT(create_material_files()));

	// --- SetUp ScrollArea ---
	scrollArea = new QScrollArea(this);
	scrollArea->setWidgetResizable(false);
	setCentralWidget(scrollArea);
	scrollArea->setWidget(ui->paintArea);

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

void MainWindow::toggleGeneratorDock() {
	ui->generatorDockWidget->setVisible(!ui->generatorDockWidget->isVisible());
}

void MainWindow::create_material_files() {
	std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!Emit Save configs" << std::endl;
	[[maybe_unused]] auto &gen = generatorConfigWidget->gen;
	/* TODO: REFACTOR THIS PLZ
		WHY YOU MUST WANTED:
		perf say as that
		 10.40%  cut              cut                            [.]
	   QUuid::compareThreeWay_helper(QUuid const&, QUuid const&)
	*/
	std::pair<std::size_t, std::size_t> factory_size = {
		2386, 828}; // gen.getFactorySize();
	std::vector<float> default_material = {
		1, 1, 1}; // gen.getDefaultMaterialsValue();
	std::vector<std::string> material_configs = {
		"main_vp.bin", "main_vs.bin",
		"main_rho.bin"}; // gen.getMaterialConfigsName();

	std::vector<std::vector<float>> material_values = {{}, {}, {}};
	// maybe we need here convert polygons coords using scale to fixed fabric
	// size
	for (std::size_t x = 0, end_x = factory_size.first; x < end_x; ++x) {
		for (std::size_t y = 0, end_y = factory_size.second; y < end_y; ++y) {
			auto polygons =
				Geometry::find_polygons_by_point({float(x), float(y)});
			if (polygons.size() == 0) {
				for (std::size_t i = 0, prop_count = default_material.size();
					 i < prop_count; ++i)
					material_values[i].push_back(default_material[i]);
			} else {
				// TODO: refactor material in polygon to vector of float
				// properties of material
				for (std::size_t i = 0, prop_count = material_configs.size();
					 i < prop_count; ++i)
					material_values[i].push_back(polygons[0]->material_);
			}
		}
	}
	// now let's just save as binary files
	for (std::size_t i = 0, end = material_configs.size(); i < end; ++i) {
		std::ofstream out(material_configs[i],
						  std::ios::out | std::ios::binary);
		out.write(reinterpret_cast<const char *>(material_values[i].data()),
				  material_values[i].size() * sizeof(float));
		out.close();
	}

	std::cout << "MainWindow end save" << std::endl;
}

MainWindow::~MainWindow() { delete ui; }
