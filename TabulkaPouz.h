#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets>

#include "ui_TabulkaPouz.h"

class TabulkaPouz : public QDialog, public Ui::TabulkaPouz
{
	Q_OBJECT

public:
	
	TabulkaPouz(QWidget* parent) : QDialog(parent), ui(new Ui::TabulkaPouz) {
		qDebug() << "kontruktor tabulkapouz";
		ui->setupUi(this);
		connect(ui->tabulka, SIGNAL(itemChanged(QTableWidgetItem*)), this, SIGNAL(ZmenaObsahuSignal()));
		connect(ui->zavriet, SIGNAL(clicked()), this, SIGNAL(ZavrietSignal()));
		connect(ui->odstrPouz, SIGNAL(clicked()), this, SIGNAL(OdstrPouzSignal()));
		connect(ui->tabulka, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)), this, SIGNAL(ZmenaRiadkuSignal()));

		setStyleSheet("background:rgb(179, 255, 224);");
		ui->tabulka->setStyleSheet("background:rgb(250, 250, 250);");
		ui->odstrPouz->setStyleSheet("background:rgb(250, 250, 250);");
		ui->zavriet->setStyleSheet("background:rgb(250, 250, 250);");
	};
	~TabulkaPouz() { delete ui;
	qDebug() << "deskruktor tabulkapouz";
	};
	void SetRows(int n) { 
		ui->tabulka->setRowCount(n);
		ui->tabulka->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	};

	void SetCell(int i, int j, QString input) {
		ui->tabulka->setItem(i, j, new QTableWidgetItem(input));
	};

	void BlockSignals(bool input) {
		ui->tabulka->blockSignals(input);
	}

	int GetCurrentRow() { return ui->tabulka->currentRow(); };
	int GetCurrentColumn() { return ui->tabulka->currentColumn(); };
	QTableWidgetItem* GetCurrentItem() { return ui->tabulka->currentItem(); };
	QString GetCurrentItemText() { return ui->tabulka->currentItem()->text(); };
	QString GetItemText(int row, int col) { return ui->tabulka->item(row, col)->text(); };
	void SetItemText(int row, int col, QString text) { ui->tabulka->item(row, col)->setText(text); };
	void SetCurrentItemText(QString input) { ui->tabulka->currentItem()->setText(input); };
	void RemoveRow(int row) { ui->tabulka->removeRow(row); };
	void EnableOdstr(bool state) { ui->odstrPouz->setEnabled(state); };

signals:
	void ZmenaObsahuSignal();
	void ZavrietSignal();
	void OdstrPouzSignal();
	void ZmenaRiadkuSignal();
private:
	Ui::TabulkaPouz* ui;
};

