#ifndef GENERATOR_CONFIG_WIDGET_H
#define GENERATOR_CONFIG_WIDGET_H

#include <QListWidgetItem>
#include <QWidget>
#include <memory>
#include <vector>

#include "correctors.h"
#include "fillers.h"
#include "generator.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class GeneratorConfigWidget;
}
QT_END_NAMESPACE

class GeneratorConfigWidget : public QWidget {
	Q_OBJECT

public:
	explicit GeneratorConfigWidget(QWidget *parent = nullptr);
	~GeneratorConfigWidget();

	terraformer::generator gen = {};

signals:
	void create_material_files();

private slots:
	void onGenerateClicked();
	void onBrowseOutputFileClicked();

	// --- Filler List Management ---
	void onAddFillerClicked();
	void onEditFillerClicked();
	void onRemoveFillerClicked();
	void onFillerDoubleClicked(
		QListWidgetItem *item); // Optional: Edit on double-click

	// --- Corrector List Management (Slots exist but are currently stubbed) ---
	// void onAddCorrectorClicked();
	// void onEditCorrectorClicked();
	// void onRemoveCorrectorClicked();
	// void onCorrectorDoubleClicked(QListWidgetItem *item);

private:
	void setupConnections();
	void setupFormsLayouts();
	void setDefaults();				  // Set initial values in UI fields
	void updateFillerListWidget();	  // Refresh list from m_fillers
	void updateCorrectorListWidget(); // Show placeholder/disabled state

	Ui::GeneratorConfigWidget *ui; // Pointer to UI definition
};

#endif // GENERATOR_CONFIG_WIDGET_H
