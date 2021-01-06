#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets>

#include "ui_NovyPouzivatelDialog.h"

class NovyPouzDialog : public QDialog, public Ui::NovyPouzDialog
{
	Q_OBJECT

public:
	
	NovyPouzDialog(QWidget* parent) : QDialog(parent), ui(new Ui::NovyPouzDialog) {
		ui->setupUi(this);
		ui->psc->setValidator(new QIntValidator);
		ui->pozicia->setVisible(false);
		ui->label_14->setVisible(false);
		//qDebug() << "konstruktor novypouzdialog";

		setStyleSheet("background:rgb(250, 250, 250);");
		ui->groupBox->setStyleSheet("background:rgb(179, 255, 224);");
		ui->login->setStyleSheet("background:rgb(250, 250, 250);");
		ui->heslo->setStyleSheet("background:rgb(250, 250, 250);");
		ui->meno->setStyleSheet("background:rgb(250, 250, 250);");
		ui->priezvisko->setStyleSheet("background:rgb(250, 250, 250);");
		ui->ulica->setStyleSheet("background:rgb(250, 250, 250);");
		ui->mesto->setStyleSheet("background:rgb(250, 250, 250);");
		ui->psc->setStyleSheet("background:rgb(250, 250, 250);");
		ui->minute->setStyleSheet("background:rgb(250, 250, 250);");
		ui->pozicia->setStyleSheet("background:rgb(250, 250, 250);");
		ui->OkButton->setStyleSheet("background:rgb(250, 250, 250);");
		ui->index->setStyleSheet("background:rgb(250,250,250);");

	};
	~NovyPouzDialog() {
		//qDebug() << "deskruktor novypouzdialog";
		delete ui;
		KontrolaChyby.clear();
	};

	int GetIndex() { return ui->index->currentIndex(); };
	QString GetLogin() { return ui->login->text(); };
	QString GetHeslo() { return ui->heslo->text(); };
	QString GetMeno() { return ui->meno->text(); };
	QString GetPriezvisko() { return ui->priezvisko->text(); };
	QString GetUlica() { return ui->ulica->text(); };
	QString GetMesto() { return ui->mesto->text(); };
	QString GetPsc() { return ui->psc->text(); };
	float GetMinute() { return ui->minute->value(); };
	QString GetPozicia() { return ui->pozicia->text(); }; 
	bool GetVybaveny() {
		if (ui->vybaveny->isChecked()) return true;
		else return false;
	};

	void SetIndex(int input) { ui->index->setCurrentIndex(input); };
	void SetLogin(QString input) { ui->login->setText(input); };
	void SetHeslo(QString input) { ui->heslo->setText(input); };
	void SetMeno(QString input) { ui->meno->setText(input); };
	void SetPriezvisko(QString input) { ui->priezvisko->setText(input); };
	void SetUlica(QString input) { ui->ulica->setText(input); };
	void SetMesto(QString input) { ui->mesto->setText(input); };
	void SetPsc(QString input) { ui->psc->setText(input); };
	void SetMinute(float input) { ui->minute->setValue(input); };
	void SetPozicia(QString input) { ui->pozicia->setText(input); };
	void SetVybaveny(bool input) { ui->vybaveny->setChecked(input); };

	void SetKontrola(int index, int value) { KontrolaChyby[index] = value; };
private:
	Ui::NovyPouzDialog* ui;
	QList<int> KontrolaChyby = { 1,1,1,1,1,1,1,0 };		//na zaciatku su vsetky prazdne, takze chybne
	//posledne je nula, lebo defaultne je nastaveny zakaznik a ten nema pracovnu poziciu


private slots:
	void on_index_currentIndexChanged(int);
	void on_login_textChanged();
	void on_heslo_textChanged();
	void on_meno_textChanged();
	void on_priezvisko_textChanged();
	void on_ulica_textChanged();
	void on_mesto_textChanged();
	void on_psc_textChanged();
	void on_pozicia_textChanged();
	void on_zobrazHeslo_clicked();
	void CheckChyby();
};