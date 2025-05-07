#include "GeneratorConfigWidget.h"
#include "ui_GeneratorConfigWidget.h"

#include <QDebug>
#include <QFileDialog>
#include <QInputDialog>
#include <QListWidget>
#include <QMessageBox>
#include <QStringList>
#include <QVariant>

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

GeneratorConfigWidget::GeneratorConfigWidget(QWidget *parent)
	: QWidget(parent), ui(new Ui::GeneratorConfigWidget) {
	ui->setupUi(this);

	setupConnections();
	setDefaults();
	setupFormsLayouts();

	// Initial display updates
	updateFillerListWidget();
	updateCorrectorListWidget(); // TODO: Will show stub message and disable
								 // buttons
}

GeneratorConfigWidget::~GeneratorConfigWidget() { delete ui; }

void GeneratorConfigWidget::setupConnections() {
	// Main actions
	connect(ui->generateButton, &QPushButton::clicked, this,
			&GeneratorConfigWidget::onGenerateClicked);
	connect(ui->browseButton, &QPushButton::clicked, this,
			&GeneratorConfigWidget::onBrowseOutputFileClicked);

	// Filler actions
	connect(ui->addFillerButton, &QPushButton::clicked, this,
			&GeneratorConfigWidget::onAddFillerClicked);
	connect(ui->editFillerButton, &QPushButton::clicked, this,
			&GeneratorConfigWidget::onEditFillerClicked);
	connect(ui->removeFillerButton, &QPushButton::clicked, this,
			&GeneratorConfigWidget::onRemoveFillerClicked);
	connect(ui->fillerListWidget, &QListWidget::itemDoubleClicked, this,
			&GeneratorConfigWidget::onFillerDoubleClicked);

	// Corrector actions (Connections are commented out as UI is stubbed)
	// connect(ui->addCorrectorButton, &QPushButton::clicked, this,
	// &GeneratorConfigWidget::onAddCorrectorClicked);
	// connect(ui->editCorrectorButton, &QPushButton::clicked, this,
	// &GeneratorConfigWidget::onEditCorrectorClicked);
	// connect(ui->removeCorrectorButton, &QPushButton::clicked, this,
	// &GeneratorConfigWidget::onRemoveCorrectorClicked);
	// connect(ui->correctorListWidget, &QListWidget::itemDoubleClicked, this,
	// &GeneratorConfigWidget::onCorrectorDoubleClicked);
}

void GeneratorConfigWidget::setupFormsLayouts() {
	ui->globalLayout->addRow(tr("Time Step (dt):"), ui->dtSpinBox);
	ui->globalLayout->addRow(tr("Number of Steps:"), ui->stepsSpinBox);

	ui->gridLayout->addRow(tr("Grid ID:"), ui->gridIdEdit);

	ui->materialLayout->addRow(tr("C1:"), ui->c1SpinBox);
	ui->materialLayout->addRow(tr("C2:"), ui->c2SpinBox);
	ui->materialLayout->addRow(tr("Rho:"), ui->rhoSpinBox);

	ui->schemaLayout->addRow(tr("Schema Name:"), ui->schemaNameEdit);

	ui->saverLayout->addRow(tr("Output Path Pattern:"), ui->saverPathEdit);
	ui->saverLayout->addRow(tr("Save Every (Steps):"), ui->stepsForSaveSpinBox);
	ui->saverLayout->addRow(tr("Parameters (comma-sep):"), ui->paramsLineEdit);
	ui->saverLayout->addRow(tr("Norms (0 or 1, comma-sep):"),
							ui->normsLineEdit);
}

void GeneratorConfigWidget::setDefaults() {
	// Set some reasonable default values in the UI fields // TODO: Replace with
	// actual defaults
	ui->dtSpinBox->setValue(0.0002);
	ui->stepsSpinBox->setValue(5000);
	ui->gridIdEdit->setText("default_grid");
	ui->c1SpinBox->setValue(1.0);
	ui->c2SpinBox->setValue(1.0);
	ui->rhoSpinBox->setValue(1.0);
	ui->sizeXSpinBox->setValue(2386);
	ui->sizeYSpinBox->setValue(828);
	ui->originXSpinBox->setValue(0);
	ui->originYSpinBox->setValue(0);
	ui->spacingXSpinBox->setValue(1.6);
	ui->spacingYSpinBox->setValue(1.6);
	ui->schemaNameEdit->setText("ElasticMatRectSchema2DRusanov3");
	ui->stepsForSaveSpinBox->setValue(100);
	ui->saverPathEdit->setText("vtk/%g_%s.vtk");
	ui->paramsLineEdit->setText("v");
	ui->normsLineEdit->setText("1");
}

void GeneratorConfigWidget::updateFillerListWidget() {
	ui->fillerListWidget->clear();
	for (const auto &filler : gen.fillers_) {
		QString desc = "Unknown Filler Type";
		// Attempt to cast to known filler types
		if (auto *rectFiller = dynamic_cast<terraformer::RectNoReflectFiller *>(
				filler.get())) {
			desc = QString("RectNoReflect: Axis=%1, Side=%2")
					   .arg(rectFiller->axis ==
									terraformer::RectNoReflectFiller::AXIS_X
								? "X (0)"
								: "Y (1)")
					   .arg(rectFiller->side ==
									terraformer::RectNoReflectFiller::SIDE_LEFT
								? "Left (0)"
								: "Right (1)");
		}
		// else if (auto* otherFiller =
		// dynamic_cast<OtherFillerType*>(filler.get())) { ... }

		QListWidgetItem *item = new QListWidgetItem(desc, ui->fillerListWidget);
		item->setData(Qt::UserRole, QVariant::fromValue(filler.get()));
	}
}

void GeneratorConfigWidget::updateCorrectorListWidget() {
	ui->correctorListWidget->clear();

	// --- STUB ---
	// Display placeholder message and disable controls
	QListWidgetItem *placeholderItem =
		new QListWidgetItem(tr("Corrector configuration under development."),
							ui->correctorListWidget);
	placeholderItem->setForeground(Qt::gray);
	placeholderItem->setFlags(placeholderItem->flags() & ~Qt::ItemIsSelectable &
							  ~Qt::ItemIsEnabled); // Make it non-interactive

	ui->addCorrectorButton->setEnabled(false);
	ui->editCorrectorButton->setEnabled(false);
	ui->removeCorrectorButton->setEnabled(false);
	ui->correctorListWidget->setEnabled(false); // Disable the whole list
												// --- END STUB ---

	// // Future implementation would be similar to updateFillerListWidget:
	// for (const auto& corrector : correctors) {
	//     QString desc = "Unknown Corrector Type";
	//     // if
	//     (dynamic_cast<terraformer::ForceRectElasticBoundary2D*>(corrector.get()))
	//     { ... }
	//     // else if
	//     (dynamic_cast<terraformer::PointSourceCorrector2D*>(corrector.get()))
	//     { ... } QListWidgetItem* item = new QListWidgetItem(desc,
	//     ui->correctorListWidget); item->setData(Qt::UserRole,
	//     QVariant::fromValue(corrector.get()));
	// }
}

void GeneratorConfigWidget::onBrowseOutputFileClicked() {
	QString currentPath = ui->outputFileEdit->text();
	QString filePath = QFileDialog::getSaveFileName(
		this, tr("Save Configuration File"),
		currentPath, // Start in current dir or last path
		tr("Configuration Files (*.conf *.cfg);;All Files (*)")); // Typical
																  // extensions
	if (!filePath.isEmpty()) {
		ui->outputFileEdit->setText(filePath);
	}
}

void GeneratorConfigWidget::onGenerateClicked() {
	qDebug() << "Generate button clicked.";

	QString outputPath = ui->outputFileEdit->text();
	if (outputPath.isEmpty()) {
		QMessageBox::warning(this, tr("Missing Output Path"),
							 tr("Please specify an output file path."));
		return;
	}

	// --- 2. Gather Values from UI and Apply to Generator ---
	// TODO: NEED TO REFACTOR GENERATOR CLASS. NEED SETTERS FOR ALL FIELDS OR
	// OTHER SETUP METHOD.
	try {
		qDebug() << "Configuring generator instance from UI...";

		// --- Global ---
		qDebug() << "--- Global ---";
		qDebug() << "dt:" << ui->dtSpinBox->value();
		qDebug() << "steps:" << ui->stepsSpinBox->value();
		gen.dt = ui->dtSpinBox->value();
		gen.nsteps = ui->stepsSpinBox->value();

		// --- Grid ---
		qDebug() << "--- Grid ---";
		qDebug() << "gridId:" << ui->gridIdEdit->text();
		gen.grid_id = ui->gridIdEdit->text().toStdString();

		// --- Material ---
		qDebug() << "--- Material ---";
		qDebug() << "c1:" << ui->c1SpinBox->value();
		qDebug() << "c2:" << ui->c2SpinBox->value();
		qDebug() << "rho:" << ui->rhoSpinBox->value();
		// gen.setMaterialC1(ui->c1SpinBox->value());
		// gen.setMaterialC2(ui->c2SpinBox->value());
		// gen.setMaterialRho(ui->rhoSpinBox->value());

		// --- Factory ---
		qDebug() << "--- Factory ---";
		qDebug() << "sizeX:" << ui->sizeXSpinBox->value();
		qDebug() << "sizeY:" << ui->sizeYSpinBox->value();
		qDebug() << "originX:" << ui->originXSpinBox->value();
		qDebug() << "originY:" << ui->originYSpinBox->value();
		qDebug() << "spacingX:" << ui->spacingXSpinBox->value();
		qDebug() << "spacingY:" << ui->spacingYSpinBox->value();
		gen.factory_->size_ = {ui->sizeXSpinBox->value(),
							   ui->sizeYSpinBox->value()};
		gen.factory_->origin_ = {ui->originXSpinBox->value(),
								 ui->originYSpinBox->value()};
		gen.factory_->spacing_ = {ui->spacingXSpinBox->value(),
								  ui->spacingYSpinBox->value()};

		// --- Schema ---
		qDebug() << "--- Schema ---";
		qDebug() << "schemaName:" << ui->schemaNameEdit->text();
		gen.schema_name = ui->schemaNameEdit->text().toStdString();

		// --- Fillers ---
		qDebug() << "--- Fillers ---";
		// gen.clearFillers();
		qDebug() << "Adding" << gen.fillers_.size()
				 << "fillers to generator configuration...";
		for (const auto &filler_ptr : gen.fillers_) {
			if (!filler_ptr)
				continue; // Should not happen

			if (auto *rectFiller =
					dynamic_cast<terraformer::RectNoReflectFiller *>(
						filler_ptr.get())) {
				qDebug() << "Adding RectNoReflectFiller: Axis="
						 << static_cast<int>(rectFiller->axis)
						 << "Side=" << static_cast<int>(rectFiller->side);
				// gen.addFiller(std::make_unique<terraformer::RectNoReflectFiller>(*rectFiller));
			}
			// else if (auto* otherFiller =
			// dynamic_cast<OtherFillerType*>(filler_ptr.get())) {
			//   qDebug() << "Adding OtherFillerType: ...";
			//    gen.addFiller(std::make_unique<OtherFillerType>(*otherFiller));
			// }
			else {
				qWarning() << "Skipping unknown filler type during generation.";
			}
		}

		// --- Correctors (Stubbed Out) ---
		// gen.clearCorrectors();
		qDebug() << "Skipping corrector configuration (UI section under "
					"development). Generator will use its defaults or an empty "
					"list if applicable.";
		// Do not add correctors from correctors here, as the UI doesn't manage
		// them yet.

		// --- Savers ---
		qDebug() << "--- Savers ---";
		qDebug() << "saverPath:" << ui->saverPathEdit->text();
		qDebug() << "stepsForSave:" << ui->stepsForSaveSpinBox->value();
		// gen.setSaverPath(ui->saverPathEdit->text().toStdString());
		// gen.setStepsForSave(ui->stepsForSaveSpinBox->value());

		// Parse comma-separated params
		qDebug() << "params:" << ui->paramsLineEdit->text();
		QString paramsText = ui->paramsLineEdit->text();
		QStringList paramsList = paramsText.split(',', Qt::SkipEmptyParts);
		std::vector<std::string> paramsVec;
		paramsVec.reserve(paramsList.size());
		for (const QString &s : paramsList) {
			paramsVec.push_back(s.trimmed().toStdString());
		}
		// qDebug() << "Parsed params:" << paramsVec;
		// gen.setSaverParams(paramsVec);

		// Parse comma-separated norms
		qDebug() << "norms:" << ui->normsLineEdit->text();
		QString normsText = ui->normsLineEdit->text();
		QStringList normsList = normsText.split(',', Qt::SkipEmptyParts);
		std::vector<size_t> normsVec;
		normsVec.reserve(normsList.size());
		bool conversionOk = true;
		for (const QString &s : normsList) {
			size_t val = s.trimmed().toULongLong(
				&conversionOk); // Or toUInt, toULong depending on expected
								// range
			if (!conversionOk) {
				throw std::runtime_error(
					"Invalid non-numeric value found in saver norms list: " +
					s.trimmed().toStdString());
			}
			normsVec.push_back(val);
		}
		qDebug() << "Parsed norms:" << normsVec;
		// gen.setSaverNorms(normsVec);

		qDebug() << "Generator configuration complete. Calling saveAs...";

		// --- 3. Call Generator's Save Method ---
		if (!outputPath.isEmpty()) {
			gen.saveAs(outputPath.toStdString());
		}

		emit create_material_files();

		QMessageBox::information(
			this, tr("Success"),
			tr("Configuration file generated successfully:\n%1")
				.arg(outputPath));

	} catch (const std::exception &e) {
		qCritical() << "Error during configuration or generation:" << e.what();
		QMessageBox::critical(this, tr("Configuration/Generation Error"),
							  tr("Error: %1").arg(e.what()));
	} catch (...) {
		qCritical() << "Unknown error during configuration or generation.";
		QMessageBox::critical(
			this, tr("Configuration/Generation Error"),
			tr("An unknown error occurred. Check console output."));
	}
}

// --- Filler List Management Slots ---

void GeneratorConfigWidget::onAddFillerClicked() { // FIXME
	// Using simple QInputDialog as a placeholder for a proper dialog.
	QStringList axisItems = {"X (0)", "Y (1)"};
	bool axisOk;
	QString axisStr =
		QInputDialog::getItem(this, tr("Add RectNoReflect Filler"), tr("Axis:"),
							  axisItems, 0, false, &axisOk);
	if (!axisOk || axisStr.isEmpty())
		return;

	QStringList sideItems = {
		"Left (0)",
		"Right (1)" // For now, only RectNoReflectFiller is known/handled.
	};
	bool sideOk;
	QString sideStr =
		QInputDialog::getItem(this, tr("Add RectNoReflect Filler"), tr("Side:"),
							  sideItems, 0, false, &sideOk);
	if (!sideOk || sideStr.isEmpty())
		return;

	// Convert selections back to enum values (assuming specific enum values)
	auto axis = (axisStr == axisItems[0])
					? terraformer::RectNoReflectFiller::AXIS_X
					: terraformer::RectNoReflectFiller::AXIS_Y;
	auto side = (sideStr == sideItems[0])
					? terraformer::RectNoReflectFiller::SIDE_LEFT
					: terraformer::RectNoReflectFiller::SIDE_RIGHT;

	// Create and add the new filler
	using namespace terraformer;
	gen.fillers_.push_back(std::unique_ptr<RectNoReflectFiller>(
		CREATE_CLASS(Filler, RectNoReflectFiller)->Init(side, axis)));
	updateFillerListWidget(); // Refresh the UI list
	qDebug() << "Added RectNoReflectFiller: Axis=" << static_cast<int>(axis)
			 << "Side=" << static_cast<int>(side) << gen.fillers_.size();
}

void GeneratorConfigWidget::onEditFillerClicked() {
	QListWidgetItem *currentItem = ui->fillerListWidget->currentItem();
	if (!currentItem) {
		QMessageBox::information(
			this, tr("Edit Filler"),
			tr("Please select a filler from the list to edit."));
		return;
	}

	// Retrieve the raw pointer stored in the item's data
	terraformer::Filler *fillerPtr =
		currentItem->data(Qt::UserRole).value<terraformer::Filler *>();
	if (!fillerPtr) {
		qWarning() << "Could not retrieve filler pointer from list item data.";
		return;
	}

	// Try to cast to known editable types
	if (auto *rectFiller =
			dynamic_cast<terraformer::RectNoReflectFiller *>(fillerPtr)) {
		qDebug() << "Editing RectNoReflectFiller: Current Axis="
				 << rectFiller->axis << "Side=" << rectFiller->side;
		// Placeholder: Use QInputDialog again for simplicity
		QStringList axisItems = {"X (0)", "Y (1)"};
		int currentAxisIndex =
			(rectFiller->axis == terraformer::RectNoReflectFiller::AXIS_X) ? 0
																		   : 1;
		bool axisOk;
		QString axisStr = QInputDialog::getItem(
			this, tr("Edit RectNoReflect Filler"), tr("Axis:"), axisItems,
			currentAxisIndex, false, &axisOk);
		if (!axisOk || axisStr.isEmpty())
			return;

		QStringList sideItems = {"Left (0)", "Right (1)"};
		int currentSideIndex =
			(rectFiller->side == terraformer::RectNoReflectFiller::SIDE_LEFT)
				? 0
				: 1;
		bool sideOk;
		QString sideStr = QInputDialog::getItem(
			this, tr("Edit RectNoReflect Filler"), tr("Side:"), sideItems,
			currentSideIndex, false, &sideOk);
		if (!sideOk || sideStr.isEmpty())
			return;

		// Update the object in fillers directly
		rectFiller->axis = (axisStr == axisItems[0])
							   ? terraformer::RectNoReflectFiller::AXIS_X
							   : terraformer::RectNoReflectFiller::AXIS_Y;
		rectFiller->side = (sideStr == sideItems[0])
							   ? terraformer::RectNoReflectFiller::SIDE_LEFT
							   : terraformer::RectNoReflectFiller::SIDE_RIGHT;

		updateFillerListWidget(); // Refresh the list to show updated text
		qDebug() << "Updated RectNoReflectFiller: New Axis=" << rectFiller->axis
				 << "New Side=" << rectFiller->side;

	}
	// else if (auto* otherFiller =
	// dynamic_cast<OtherEditableFiller*>(fillerPtr)) { ... }
	else {
		QMessageBox::information(
			this, tr("Edit Filler"),
			tr("Editing for this type of filler is not implemented yet."));
	}
}

void GeneratorConfigWidget::onRemoveFillerClicked() {
	QListWidgetItem *currentItem = ui->fillerListWidget->currentItem();
	if (!currentItem) {
		QMessageBox::information(
			this, tr("Remove Filler"),
			tr("Please select a filler from the list to remove."));
		return;
	}

	terraformer::Filler *fillerPtr =
		currentItem->data(Qt::UserRole).value<terraformer::Filler *>();
	if (!fillerPtr) {
		qWarning() << "Could not retrieve filler pointer from list item data "
					  "for removal.";
		return;
	}

	// Find and remove the corresponding unique_ptr from fillers
	auto it = std::remove_if(
		gen.fillers_.begin(), gen.fillers_.end(),
		[fillerPtr](const std::unique_ptr<terraformer::Filler> &p) {
			return p.get() == fillerPtr;
		});

	if (it != gen.fillers_.end()) {
		gen.fillers_.erase(it, gen.fillers_.end());
		qDebug() << "Removed filler from internal list." << gen.fillers_.size();
		updateFillerListWidget(); // Refresh the UI list
	} else {
		qWarning() << "Could not find the selected filler in the internal list "
					  "for removal.";
	}
}

void GeneratorConfigWidget::onFillerDoubleClicked(QListWidgetItem *item) {
	if (item) {
		// Treat double-click same as selecting and clicking edit
		onEditFillerClicked();
	}
}
