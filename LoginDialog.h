#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets>

#include "ui_login.h"

class LoginDialog : public QDialog, public Ui::LoginOkno
{
	Q_OBJECT

public:

	LoginDialog(QWidget* parent) : QDialog(parent), ui(new Ui::LoginOkno) {
		
		ui->setupUi(this);

		//qDebug() << "kontruktor login";
		setStyleSheet("background:rgb(250, 250, 250);");
		ui->groupBox->setStyleSheet("background:rgb(179, 255, 224);");
		ui->linelogin->setStyleSheet("background:rgb(250, 250, 250);");
		ui->lineheslo->setStyleSheet("background:rgb(250, 250, 250);");
	};
	~LoginDialog() { delete ui;
		//qDebug() << "deskruktor login";
	};

	QString GetLogin() { return ui->linelogin->text(); };
	QString GetHeslo() { return ui->lineheslo->text(); };
	void SetLogin(QString L) { ui->linelogin->setText(L); };
	void SetHeslo(QString H) { ui->lineheslo->setText(H); };
	void DesignLogin();
signals:
	void aboutToQuit();
private:
	Ui::LoginOkno* ui;
	void closeEvent(QCloseEvent* event) { aboutToQuit(); };

private slots:
	void on_pswdcheck_clicked() {				//skryvanie hesla pri pisani
		if (ui->pswdcheck->isChecked()) ui->lineheslo->setEchoMode(QLineEdit::Normal);
		else  ui->lineheslo->setEchoMode(QLineEdit::Password);
	};
};


