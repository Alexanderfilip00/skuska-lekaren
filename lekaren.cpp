#pragma once
#include "lekaren.h"


lekaren::lekaren(QWidget *parent)
    : QMainWindow(parent)
{
    QFont font("Roboto Light");
    font.setPointSize(10);
    QApplication::setFont(font);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    UploadPouz();
    UploadLieky("lieky.csv");
    UploadKosiky();
    
    //##############################################################################################################################################
    LoginWindow = new LoginDialog(parent);            //spusti login okno   //parent namiesto this, aby sa to zobrazilo v taskbare
    
    connect(LoginWindow, SIGNAL(aboutToQuit()), this, SLOT(Exit()));        //X-nutie okna
    connect(LoginWindow, SIGNAL(accepted()), this, SLOT(KontrolaLogin()));
    connect(LoginWindow, SIGNAL(rejected()), this, SLOT(Exit()));
    LoginWindow->exec();
}



void lekaren::on_Odhlasit_clicked()
{
    //#############-POUZIVATELIA-#############//

    if (CheckZmeny(2)) {
        int i;

        QFile file("pouzivatelia.csv");                                       //zapisanie do suboru
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;

        QTextStream out(&file);
        out << "#pouzivatelia#,";

        for (i = 0; i < Pouzivatelia.size(); i++) {
            out << "\n" << Pouzivatelia[i]->GetIndex() << "," << Pouzivatelia[i]->GetLogin() << "," << Pouzivatelia[i]->GetHeslo() << "," << Pouzivatelia[i]->GetMeno() << "," << Pouzivatelia[i]->GetPriezvisko() << ",";
            //vsetkym vypise zaklad

            if (Pouzivatelia[i]->GetIndex() == 0) {     //zakaznikom dopise fakturacnu adresu a dalsie info
                out << Pouzivatelia[i]->GetUlica() << "," << Pouzivatelia[i]->GetMesto() << "," << Pouzivatelia[i]->GetPsc() << "," << Pouzivatelia[i]->GetMinute() << "," << Pouzivatelia[i]->GetVybaveny() << ",";
            }
            else if (Pouzivatelia[i]->GetIndex() == 1) {        //zamestnancom dopise poziciu
                out << Pouzivatelia[i]->GetPozicia() << ",";
            }

        }

        file.close();
    }

    //#############-ZOZNAM LIEKOV-#############//

    if (CheckZmeny(3)) {

        int i;
        int j;

        QFile file("pouzkosiky.csv");                                       //zapisanie do suboru
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;

        QTextStream out(&file);
        out << "#databaza_kosikov_pouzivatelov#,";


        for (i = 0; i < Pouzivatelia.size(); i++) {
            if (Pouzivatelia[i]->GetIndex() == 0) {
                out << "\n" << i << ",";      //login pouzivatela

                for (j = 0; j < Pouzivatelia[i]->GetLiekyDataSize(); j++) {
                    out << Pouzivatelia[i]->GetLiekyDataIndex(j) << ",";        //obsah kosika pouzivatela
                }
            }
        }
        file.close();
    }


    delete this;        //zavolam destruktor sucasnej lekarne
    lekaren* m = new lekaren;       //dynamicky vytvorim novu ukazem ju
    m->show();
}

void lekaren::closeEvent(QCloseEvent* event) {
    if (CheckZmeny(1)) {
        QMessageBox::StandardButton Quit;
        Quit = QMessageBox::question(this, "Prázdna bunka", "Niektoré bunky sú prázdne. Prajete si opustiť aplikáciu bez uloženia?",
            QMessageBox::Yes | QMessageBox::No);
        if (Quit == QMessageBox::Yes) {
            Exit();
        }
        else {
            event->ignore();
        }

    }
    else {
        ui.Odhlasit->click();
        event->accept();
    }
}

lekaren::~lekaren()
{
    //qDebug() << "deskruktor lekarne";
    int i;
    for (i = 0; i < Pouzivatelia.size(); i++) {
        delete Pouzivatelia[i];
    }
    delete LoginWindow;
    delete DIALOGnovyPouzivatel;
    delete DIALOGpouz;

}

QString lekaren::PswdEnc(QString heslo)
{
    QString result = QCryptographicHash::hash(heslo.toStdString().c_str(), QCryptographicHash::Sha1).toHex();
    return result;
}

QVector<int> lekaren::ConvertToInt(QVector<QString> *input)
{
    QVector<int> output;
    int i;
    
    for (i = 0; i < input->size(); i++) {
        output.push_back(input->at(i).toInt());
    }
    return output;
}

bool lekaren::LiekyContains(QString meno)
{
    int i;
    int pocet = Lieky.size();
    for (i = 0; i < pocet; i++) {
        if (Lieky[i].GetMeno() == meno) return true;
    }
    return false;
}

int lekaren::LiekyIndexOf(QString meno)
{
    int i;
    int pocet = Lieky.size();
    for (i = 0; i < pocet; i++) {
        if (Lieky[i].GetMeno() == meno) return i;
    }
    return -1;
}

int lekaren::PouzivateliaIndexOfLogin(QString login)
{
    int i;
    int pocet = Pouzivatelia.size();
    for (i = 0; i < pocet; i++) {
        if (Pouzivatelia[i]->GetLogin() == login) return i;
    }
    return -1;
}

bool lekaren::CheckHesloAt(int index, QString input)
{
    if (Pouzivatelia[index]->GetHeslo() == input) return true;
    else return false;
}

void lekaren::on_NovyPouz_pressed()
{
    DIALOGnovyPouzivatel = new NovyPouzDialog(this);
    connect(DIALOGnovyPouzivatel, SIGNAL(accepted()), this, SLOT(NovyPouzPridany()));
    DIALOGnovyPouzivatel->exec();
}

void lekaren::PrisposobUI()
{
    int i;
    ui.setupUi(this);
    DesignMain();
    
    ui.psc->setValidator(new QIntValidator);
    ui.meno->setText(Prihlaseny->GetMeno());         //plati pre vsetkych
    ui.priezvisko->setText(Prihlaseny->GetPriezvisko());
    AktualizujPonuku();

    for (i = 0; i < Pouzivatelia.size(); i++) {     //prida moznosti zobrazenia kosikov, vhodne hlavne pre zamestnanca a admina
        ui.vyberPouzivatela->addItem(Pouzivatelia[i]->GetLogin());
    }

    if (Prihlaseny->GetIndex() == 0) {       //zakaznik

        if (Prihlaseny->GetZlava() != 0.0) {
            ui.label_10->setText("Prémiový zákaznik");
        }
        else {
            ui.label_10->setText("Zákaznik");
        }
        
        ui.ulica->setText(Prihlaseny->GetUlica());
        ui.mesto->setText(Prihlaseny->GetMesto());
        ui.psc->setText(Prihlaseny->GetPsc());
        ui.zlava->setValue(Prihlaseny->GetZlava());

        ui.notAdmin->setVisible(true);
        ui.pozicia->setVisible(false);
        ui.label_7->setVisible(false);
        ui.label_6->setVisible(true);
        ui.zlava->setVisible(true);

        ui.Vytlacit->setVisible(false);
        ui.Pripravene->setVisible(false);
        ui.NovyPouz->setVisible(false);
        ui.UprPouz->setVisible(false);
        ui.UploadZoznam->setVisible(false);
        ui.vyberPouzivatela->setVisible(false);
        ui.poziciaVybrany->setVisible(false);
        ui.groupBox_5->setVisible(false);

        ui.vyberPouzivatela->setCurrentIndex(PouzivateliaIndexOfLogin(Prihlaseny->GetLogin()));

        if (Prihlaseny->GetVybaveny()) {       //ak zakaznik je vybaveny
            ui.LiekyPredaj->setEnabled(false);
            ui.LiekyKosik->setEnabled(false);
            ui.kosikPridat->setEnabled(false);
            ui.kosikOdobrat->setEnabled(false);
        }
        else {
            ui.LiekyPredaj->setEnabled(true);
            ui.LiekyKosik->setEnabled(true);
            ui.kosikPridat->setEnabled(true);
            ui.kosikOdobrat->setEnabled(true);
        }
    }
    else if (Prihlaseny->GetIndex() == 1) {      //zamestnanec
        ui.label_10->setText("Zamestnanec");
        ui.ulica->setText(Prihlaseny->GetUlica());
        ui.mesto->setText(Prihlaseny->GetMesto());
        ui.psc->setText(Prihlaseny->GetPsc());
        ui.pozicia->setText(Prihlaseny->GetPozicia());

        ui.notAdmin->setVisible(true);
        ui.pozicia->setVisible(true);
        ui.label_7->setVisible(true);
        ui.label_6->setVisible(false);
        ui.zlava->setVisible(false);
        ui.kosikPridat->setVisible(false);
        ui.kosikOdobrat->setVisible(false);
        ui.groupBox_6->setVisible(false);

        ui.label_3->setVisible(false);
        ui.ulica->setVisible(false);
        ui.label_4->setVisible(false);
        ui.mesto->setVisible(false);
        ui.label_5->setVisible(false);
        ui.psc->setVisible(false);

        ui.NovyPouz->setVisible(false);
        ui.UprPouz->setVisible(false);
        ui.UploadZoznam->setVisible(false);
        ui.vyberPouzivatela->setVisible(true);
        ui.poziciaVybrany->setVisible(true);
    }
    else if (Prihlaseny->GetIndex() == 2) {      //admin
        ui.label_10->setText("Admin");
        ui.notAdmin->setVisible(false);
        ui.UploadZoznam->setVisible(true);
        ui.vyberPouzivatela->setVisible(true);
        ui.poziciaVybrany->setVisible(true);
    }

    ui.LiekyPredaj->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui.LiekyKosik->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui.kosikPridat->setEnabled(false);
    ui.kosikOdobrat->setEnabled(false);
    AktualizujKosik();
}

bool lekaren::KontrolaTextu(QString input)
{
    if (input.contains(",") || input.contains("\\") ){
        return true;
    }
    else {
        return false;
    }
}

void lekaren::NovyPouzPridany() {
    int Tindex = DIALOGnovyPouzivatel->GetIndex();
    QString Tlogin = DIALOGnovyPouzivatel->GetLogin();
    QString Theslo = PswdEnc(DIALOGnovyPouzivatel->GetHeslo());
    QString Tmeno = DIALOGnovyPouzivatel->GetMeno();
    QString Tpriezvisko = DIALOGnovyPouzivatel->GetPriezvisko();

    QString Tulica = DIALOGnovyPouzivatel->GetUlica();
    QString Tmesto = DIALOGnovyPouzivatel->GetMesto();
    QString Tpsc = DIALOGnovyPouzivatel->GetPsc();
    float Tminute = DIALOGnovyPouzivatel->GetMinute();
    bool Tvybaveny = DIALOGnovyPouzivatel->GetVybaveny();
    
    QString Tpozicia = DIALOGnovyPouzivatel->GetPozicia();
    
    if (PouzivateliaIndexOfLogin(Tlogin) == -1) {      //ak neexistuje zhodny login

        QFile file("pouzivatelia.csv");                                       //zapisanie do suboru
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
            QMessageBox msgBox;
            msgBox.setText("Nepodarilo sa uložiť nového používateľa.");
            msgBox.exec();
            return;
        }

        QFile fileKosiky("pouzkosiky.csv");                                       //zapisanie do suboru
        if (!fileKosiky.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
            QMessageBox msgBox;
            msgBox.setText("Nepodarilo sa uložiť nového používateľa.");
            msgBox.exec();
            return;
        }

        QTextStream out(&file);
        QTextStream outKosiky(&fileKosiky);

        if (Tindex == 0) {      //zakaznik
            outKosiky << "\n" << Pouzivatelia.size() << ",";      //napisem poradie v zozname vsetkych pouzivatelov, ktore nadobudne
            out << "\n" << Tindex << "," << Tlogin << "," << Theslo << "," << Tmeno << "," << Tpriezvisko << "," << Tulica << "," << Tmesto << "," << Tpsc << "," << Tminute << "," << Tvybaveny << ",";
            Pouzivatelia.push_back(new zakaznik(Tindex, Tlogin, Theslo, Tmeno, Tpriezvisko, Tulica, Tmesto, Tpsc, Tminute, Tvybaveny));
        }
        else if (Tindex == 1) {     //zamestnanec
            out << "\n" << Tindex << "," << Tlogin << "," << Theslo << "," << Tmeno << "," << Tpriezvisko << "," << Tpozicia << ",";
            Pouzivatelia.push_back(new zamestnanec(Tindex, Tlogin, Theslo, Tmeno, Tpriezvisko, Tpozicia));
        }
        else {      //admin
            Pouzivatelia.push_back(new admin(Tindex, Tlogin, Theslo, Tmeno, Tpriezvisko));
            out << "\n" << Tindex << "," << Tlogin << "," << Theslo << "," << Tmeno << "," << Tpriezvisko << ",";
        }
        
        file.close();
        fileKosiky.close();
        ui.vyberPouzivatela->addItem(Tlogin);     //pridam noveho pouzivatela do zoznamu pre vyber kosikov

        SetZmeny(10, 2);     //nastavim, aby ocakavalo zmenu medzi pouzivatelmi a teda prepisalo zoznam pouzivatelov
        SetZmeny(6, 3);     //nastavim, aby ocakavalo zmenu v kosiku a teda prepisalo zoznam kosikov
    }
    else {
        QMessageBox msgBox;
        msgBox.setText("Zadaný login už existuje.");
        msgBox.exec();

        DIALOGnovyPouzivatel = new NovyPouzDialog(this);
        connect(DIALOGnovyPouzivatel, SIGNAL(accepted()), this, SLOT(NovyPouzPridany()));

        DIALOGnovyPouzivatel->SetIndex(Tindex);     //nastavy hodnoty tak, ako boli pred kliknutim OK
        DIALOGnovyPouzivatel->SetLogin(Tlogin);

        DIALOGnovyPouzivatel->SetMeno(Tmeno);
        DIALOGnovyPouzivatel->SetPriezvisko(Tpriezvisko);

        if (Tindex == 0) {

            DIALOGnovyPouzivatel->SetUlica(Tulica);
            DIALOGnovyPouzivatel->SetMesto(Tmesto);
            DIALOGnovyPouzivatel->SetPsc(Tpsc);
            DIALOGnovyPouzivatel->SetMinute(Tminute);
            if (Tvybaveny == true) DIALOGnovyPouzivatel->SetVybaveny(true);
            else DIALOGnovyPouzivatel->SetVybaveny(false);
        }
        else if (Tindex == 1) {
            DIALOGnovyPouzivatel->SetPozicia(Tpozicia);
        }
        
        DIALOGnovyPouzivatel->exec();       //spusti okno znova
    }
}

void lekaren::KontrolaLogin()
{
    QString Tlogin = LoginWindow->GetLogin();       //ulozi si vlozeny login a zasifrovane heslo
    QString Theslo = PswdEnc(LoginWindow->GetHeslo());
    int PrihlasenyIndex = PouzivateliaIndexOfLogin(Tlogin);      //zisti, ci je login v zozname loginov
   
    
    if (PrihlasenyIndex == -1) {     //ak login neexistuje, upozorni to a spusti okno znova

        QMessageBox msgBox;
        msgBox.setText("Nesprávny login. \n");
        msgBox.exec();

        LoginWindow = new LoginDialog(this);
        LoginWindow->SetLogin(Tlogin);
        
        connect(LoginWindow, SIGNAL(accepted()), this, SLOT(KontrolaLogin()));
        connect(LoginWindow, SIGNAL(rejected()), this, SLOT(Exit()));
        LoginWindow->exec();
    }
    else {
        if (CheckHesloAt(PrihlasenyIndex,Theslo)) {       //inak skontroluje heslo, ak sedi, spusti hlavne okno
            Prihlaseny = Pouzivatelia[PrihlasenyIndex];
            qDebug() << PrihlasenyIndex;
            PrisposobUI();
            
        }
        else {                  //ak nesedi, vypise varovanie a znova spusti login okno
            QMessageBox msgBox;
            msgBox.setText("Nesprávne heslo. \n");
            msgBox.exec();

            LoginWindow = new LoginDialog(this);
            LoginWindow->SetLogin(Tlogin);
            
            connect(LoginWindow, SIGNAL(accepted()), this, SLOT(KontrolaLogin()));
            connect(LoginWindow, SIGNAL(rejected()), this, SLOT(Exit()));
            LoginWindow->exec();
        }
    }
}

void lekaren::Exit()
{
    delete this;
    exit(0);
}

void lekaren::check_Odhlasit()
{
    if (CheckZmeny(1)) {
        ui.Odhlasit->setEnabled(false);
    }
    else {
        ui.Odhlasit->setEnabled(true);
    }
}

void lekaren::on_UprPouz_clicked()
{
    int i;
    DIALOGpouz = new TabulkaPouz(this);  
    DIALOGpouz->SetRows(Pouzivatelia.size());
    DIALOGpouz->BlockSignals(true);
    for (i = 0; i < Pouzivatelia.size(); i++) {

        if(Pouzivatelia[i]->GetIndex() == 0){
            if (Pouzivatelia[i]->GetZlava() == 0.0) {
                DIALOGpouz->SetCell(i, 0, "Zákazník");
            }
            else {
                DIALOGpouz->SetCell(i, 0, "Prémiový zákazník");
            }
        }
        else if (Pouzivatelia[i]->GetIndex() == 1) {
            DIALOGpouz->SetCell(i, 0, "Zamestnanec");
        }
        else {
            DIALOGpouz->SetCell(i, 0, "Admin");
        }
            
        
        DIALOGpouz->SetCell(i, 1, Pouzivatelia[i]->GetLogin());
        DIALOGpouz->SetCell(i, 2, "********");      //nezobrazujem heslo, len povolujem ho menit
        DIALOGpouz->SetCell(i, 3, Pouzivatelia[i]->GetMeno());
        DIALOGpouz->SetCell(i, 4, Pouzivatelia[i]->GetPriezvisko());

        if (Pouzivatelia[i]->GetIndex() == 0) {     //zakaznik
            DIALOGpouz->SetCell(i, 5, Pouzivatelia[i]->GetUlica());
            DIALOGpouz->SetCell(i, 6, Pouzivatelia[i]->GetMesto());
            DIALOGpouz->SetCell(i, 7, Pouzivatelia[i]->GetPsc());
            DIALOGpouz->SetCell(i, 8, QString::number(Pouzivatelia[i]->GetMinute()));
            DIALOGpouz->SetCell(i, 9, "--");
            QString Tvybaveny;
            if (Pouzivatelia[i]->GetVybaveny()) Tvybaveny = "1";
            else  Tvybaveny = "0";
            DIALOGpouz->SetCell(i, 10, Tvybaveny);
        }
        else {
            DIALOGpouz->SetCell(i, 5, "--");
            DIALOGpouz->SetCell(i, 6, "--");
            DIALOGpouz->SetCell(i, 7, "--");
            DIALOGpouz->SetCell(i, 8, "--");
            DIALOGpouz->SetCell(i, 9, Pouzivatelia[i]->GetPozicia());
            DIALOGpouz->SetCell(i, 10, "--");
        }
        
        if (Pouzivatelia[i]->GetIndex() == 2) {
            DIALOGpouz->SetCell(i, 9, "--");        //pri adminovi prepise prazdnu poziciu na spravny format "--"
        }
  
    }
    DIALOGpouz->BlockSignals(false);

    connect(DIALOGpouz, SIGNAL(ZmenaObsahuSignal()), this, SLOT(ZmenaObsahuTabulky()));
    connect(DIALOGpouz, SIGNAL(ZavrietSignal()), this, SLOT(ZatvorDialogPouz()));
    connect(DIALOGpouz, SIGNAL(OdstrPouzSignal()), this, SLOT(OdstranenyPouz()));
    connect(DIALOGpouz, SIGNAL(ZmenaRiadkuSignal()), this, SLOT(TabPouzZmenaRiadku()));
    DIALOGpouz->exec();    
}

void lekaren::on_Vytlacit_clicked()
{
    int i;
    QString Datum = QDateTime::currentDateTime().toString("dd.MM.yyyy");
    QString Cas = QDateTime::currentDateTime().toString("hh:mm:ss");

    int VyberIndex = ui.vyberPouzivatela->currentIndex();
    QString Subor = ui.vyberPouzivatela->currentText() + ".txt";

    QFile file(Subor);                                       //zapisanie do suboru
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    out << "\n     ---------------------------     ";
    out << "\n-----###########################-----\n\n\n";
    out << QString::fromStdString(std::string(9, ' ')) << Datum << " " << Cas;
    if (Pouzivatelia[VyberIndex]->GetZlava() == 0.0) {
        out << "\n\nZákazník \n \n";
    }
    else {
        out << "\n\n" << QString::fromStdString(std::string(10, ' ')) << QString::QString("Prémiový zakazník\n");
        out << QString::fromStdString(std::string(7, ' ')) << QString::QString("(dostupná zľava: ") << QString::number(Pouzivatelia[VyberIndex]->GetZlava(),'f',2) << "% ) \n\n";
    }
    out << QString::QString(" krstné meno: ") << Pouzivatelia[VyberIndex]->GetMeno() << "\n";
    out << " priezvisko: " << Pouzivatelia[VyberIndex]->GetPriezvisko() << "\n\n";
    out << QString::fromStdString(std::string(10, ' ')) << QString::QString("Fakturačná adresa:\n ") << Pouzivatelia[VyberIndex]->GetUlica() << ", " << Pouzivatelia[VyberIndex]->GetMesto() << ", " << Pouzivatelia[VyberIndex]->GetPsc() << "\n\n";
    out << QString::QString("######## - Nakúpený tovar: - ########\n\n");
    
    for (i = 0; i < Pouzivatelia[VyberIndex]->GetLiekyDataSize(); i++) {
        int index_lieku = Pouzivatelia[VyberIndex]->GetLiekyDataIndex(i);
        out << " " << Lieky[index_lieku].GetMeno().leftJustified(25, ' ') << " " << QString::number(Lieky[index_lieku].GetCena()).rightJustified(10,' ') << "\n";
    }
    out << "\n#####################################\n\n";
    
    out << QString::QString("\n\n Cena nákupu: ") << QString::number(ui.cena->value()).rightJustified(22, ' ') << "\n";
    if (Pouzivatelia[VyberIndex]->GetZlava() != 0.0) {
        out << QString::QString("\n Cena nákupu po zľave: ") << QString::number(ui.cenaPoZlave->value()).rightJustified(13, ' ') << "\n";
    }
    out << "\n\n-----###########################-----";
    out << "\n     ---------------------------     ";

    file.close();

    QMessageBox msgBox;
    msgBox.setText("Výpis bol úspešne exportovaný.");
    msgBox.exec();
}

void lekaren::on_UploadZoznam_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open file", "", tr(".csv File (*csv);;.txt File (*txt);;All files (*.*)"));
    if (fileName.isEmpty()) { return; }

    UploadLieky(fileName);

    QMessageBox msgBox;
    msgBox.setText("Databáza liekov úspešne aktualizovaná.");
    msgBox.exec();
}

void lekaren::on_Pripravene_clicked()
{
    SetZmeny(8, 2);      //nastavim spravu o zmene v pouzivatelovi
    if (Pouzivatelia[ui.vyberPouzivatela->currentIndex()]->GetVybaveny()) {             //ak bol vybaven�
        Pouzivatelia[ui.vyberPouzivatela->currentIndex()]->SetVybaveny(false);       //obrati stav vybavenosti
        ui.Pripravene->setText("Označiť ako Pripravené");
        ui.kosikPridat->setEnabled(false);
        ui.kosikOdobrat->setEnabled(false);

        ui.LiekyKosik->setEnabled(true);
        ui.LiekyPredaj->setEnabled(true);
    }
    else {                   //ak nebol vybaveny
        Pouzivatelia[ui.vyberPouzivatela->currentIndex()]->SetVybaveny(true);
        ui.Pripravene->setText("Označiť ako Nepripravené");
        ui.kosikPridat->setEnabled(false);                      //ak je objednavka pripravena, nemoze sa menit
        ui.kosikOdobrat->setEnabled(false);

        ui.LiekyKosik->setEnabled(false);
        ui.LiekyPredaj->setEnabled(false);
    }
}

void lekaren::on_meno_textChanged()
{
    if (ui.meno->text() == "") {
        SetZmeny(0, 1);
        ui.meno->setStyleSheet("QLineEdit {background-color: red;}");
    }
    else if (ui.meno->text() == Prihlaseny->GetMeno()) {
        SetZmeny(0, 0);      //stane sa velmi ojedinele: iba ak bol jeden znak, vymaze sa a znova sa napise
        ui.meno->setStyleSheet("QLineEdit {background-color: white;}");
    }
    else {
        Prihlaseny->SetMeno(ui.meno->text());
        SetZmeny(0, 2);
        ui.meno->setStyleSheet("QLineEdit {background-color: white;}");
    }
    check_Odhlasit();
}

void lekaren::on_priezvisko_textChanged()
{
    if (ui.priezvisko->text() == "") {
        ui.priezvisko->setStyleSheet("QLineEdit {background-color: red;}");     //nastavenie policka na cervenu
        SetZmeny(1, 1);      //ulozim si signal 1, cize chyba
    }
    else if (ui.priezvisko->text() == Prihlaseny->GetPriezvisko()) {
        SetZmeny(1, 0);      //na kontrole zmien necham signal 0, cize ziadna zmena
        ui.priezvisko->setStyleSheet("QLineEdit {background-color: white;}");
    }
    else {
        Prihlaseny->SetPriezvisko(ui.priezvisko->text());
        SetZmeny(1, 2);         //na kontrole chyb dam 2, cize uprava
        ui.priezvisko->setStyleSheet("QLineEdit {background-color: white;}");
    }
    check_Odhlasit();
}

void lekaren::on_ulica_textChanged()
{
    if (ui.ulica->text() == "") {
        SetZmeny(2, 1);
        ui.ulica->setStyleSheet("QLineEdit {background-color: red;}");
    }
    else if (ui.ulica->text() == Prihlaseny->GetUlica()) {
        SetZmeny(2, 0);      //stane sa velmi ojedinele: iba ak bol jeden znak, vymaze sa a znova sa napise
        ui.ulica->setStyleSheet("QLineEdit {background-color: white;}");
    }
    else {
        Prihlaseny->SetUlica(ui.ulica->text());
        SetZmeny(2, 2);
        ui.ulica->setStyleSheet("QLineEdit {background-color: white;}");
    }
    check_Odhlasit();
}

void lekaren::on_mesto_textChanged()
{
    if (ui.mesto->text() == "") {
        SetZmeny(3, 1);
        ui.mesto->setStyleSheet("QLineEdit {background-color: red;}");
    }
    else if (ui.mesto->text() == Prihlaseny->GetMesto()) {
        SetZmeny(3, 0);  
        ui.mesto->setStyleSheet("QLineEdit {background-color: white;}");
    }
    else {
        Prihlaseny->SetMesto(ui.mesto->text());
        SetZmeny(3, 2);
        ui.mesto->setStyleSheet("QLineEdit {background-color: white;}");
    }
    check_Odhlasit();
}

void lekaren::on_psc_textChanged()
{
    if (ui.psc->text() == "") {
        SetZmeny(4, 1);
        ui.psc->setStyleSheet("QLineEdit {background-color: red;}");
    }
    else if (ui.psc->text() == Prihlaseny->GetPsc()) {
        SetZmeny(4, 0);  
        ui.psc->setStyleSheet("QLineEdit {background-color: white;}");
    }
    else {
        Prihlaseny->SetPsc(ui.psc->text());
        SetZmeny(4, 2);
        ui.psc->setStyleSheet("QLineEdit {background-color: white;}");
    }
    check_Odhlasit();
}

void lekaren::on_pozicia_textChanged()
{
    if (ui.pozicia->text() == "") {
        SetZmeny(5, 1);     
        ui.pozicia->setStyleSheet("QLineEdit {background-color: red;}");
    }
    else if (ui.pozicia->text() == Prihlaseny->GetPozicia()) {
        SetZmeny(5, 0);
        ui.pozicia->setStyleSheet("QLineEdit {background-color: white;}");
    }
    else {
        Prihlaseny->SetPozicia(ui.pozicia->text());
        SetZmeny(5, 2);
        ui.pozicia->setStyleSheet("QLineEdit {background-color: white;}");
    }
    check_Odhlasit();
}

void lekaren::on_HladatLiek_textChanged()
{
    VysledkyHladania.clear();

    if (ui.HladatLiek->text() == "") {
        VysledkyHladania.push_back(-1);
    }
    else {
        int i;
        
        for (i = 0; i < Lieky.size(); i++) {
           
            if (Lieky[i].GetMeno().contains(ui.HladatLiek->text(), Qt::CaseInsensitive)) {
                VysledkyHladania.push_back(i);      //do vysledkov napisem index daneho lieku
            }
        }
    }
    AktualizujPonuku();
    ZafarbiObchod();
}

void lekaren::on_kosikPridat_clicked()
{
    QString Tmeno = ui.LiekyPredaj->item(ui.LiekyPredaj->currentRow(), 0)->text();
    int index = LiekyIndexOf(Tmeno);
 
    Pouzivatelia[ui.vyberPouzivatela->currentIndex()]->LiekyDataAppend(index);   //prida index do zoznamu nakupenych
 
    ui.kosikPridat->setEnabled(false);
    ui.LiekyPredaj->clearSelection();

    ui.LiekyPredaj->blockSignals(true);
    ui.LiekyPredaj->setCurrentCell(-1, -1);
    ui.LiekyPredaj->blockSignals(false);
    SetZmeny(7, 3);      //nastavi na index 7 chybu 3, cize zmena kosika
    AktualizujKosik();  
}

void lekaren::on_LiekyPredaj_currentCellChanged()
{
    if (!VysledkyHladania.isEmpty()) {      //funguje to, iba ak su zobrazene nejake lieky

        QString nazovLieku = ui.LiekyPredaj->item(ui.LiekyPredaj->currentRow(), 0)->text();
        int index = LiekyIndexOf(nazovLieku);

        if (Pouzivatelia[ui.vyberPouzivatela->currentIndex()]->GetIndex() == 1 || Pouzivatelia[ui.vyberPouzivatela->currentIndex()]->GetIndex() == 2) {
            ui.kosikPridat->setEnabled(false);      //ak je vybraty pouzivatel zamestnanec alebo admin, nemozem mu menit kosik
            ui.kosikOdobrat->setEnabled(false);
        }
        else {
            if (Pouzivatelia[ui.vyberPouzivatela->currentIndex()]->GetVybaveny() == true) {     //skontroluje, ci nie je objednavka vybavena, vtedy ju zakaze menit
                ui.kosikPridat->setEnabled(false);
            }
            else{
                if (Pouzivatelia[ui.vyberPouzivatela->currentIndex()]->LiekyDataContains(index)) {              //skontroluje, ci take uz nema pouzivatel kupene
                    ui.kosikPridat->setEnabled(false);
                }
                else {
                    ui.kosikPridat->setEnabled(true);
                }
            }
        }
    }
}

void lekaren::on_kosikOdobrat_clicked()
{  
    int index = LiekyIndexOf(ui.LiekyKosik->item(ui.LiekyKosik->currentRow(), 0)->text());
    Pouzivatelia[ui.vyberPouzivatela->currentIndex()]->LiekyDataRemove(index);
 
    AktualizujKosik();
    ui.kosikOdobrat->setEnabled(false);
    ui.LiekyKosik->clearSelection();

    ui.LiekyKosik->blockSignals(true);
    ui.LiekyKosik->setCurrentCell(-1, -1);
    ui.LiekyKosik->blockSignals(false);

    SetZmeny(6, 3);      //nastavi na indexe 6 chybu 3, cize zmena kosika
}

void lekaren::on_LiekyKosik_currentCellChanged()
{
    if (Pouzivatelia[ui.vyberPouzivatela->currentIndex()]->GetVybaveny() == true) {     //skontroluje, ci nie je objednavka vybavena, vtedy ju zakaze menit
        ui.kosikOdobrat->setEnabled(false);
    }
    else ui.kosikOdobrat->setEnabled(true); 
    
}

void lekaren::on_vyberPouzivatela_currentIndexChanged(int)
{
    if (Pouzivatelia[ui.vyberPouzivatela->currentIndex()]->GetIndex() == 1 || Pouzivatelia[ui.vyberPouzivatela->currentIndex()]->GetIndex() == 2) {
        ui.Pripravene->setEnabled(false);       //ak je to pracovnik alebo admin, neda sa mu menit stav vybavenosti
        ui.kosikPridat->setEnabled(false);      
        ui.kosikOdobrat->setEnabled(false);        //neda sa ani pridat a odobrat produkt z kosika
        ui.Vytlacit->setEnabled(false);             //ani tlacit blocek, ani vidiet cenu nakupu
        ui.cenyGroup->setVisible(false);
        ui.LiekyKosik->setEnabled(false);

        ui.LiekyPredaj->setEnabled(true);   //ponuka je vzdy dostupna

        ui.LiekyPredaj->setColumnHidden(2, true);       //skryje sa stlpec so zlavou
        ui.LiekyKosik->setColumnHidden(2, true);
    }
    else {                      //ak je to zakaznik
        ui.Pripravene->setEnabled(true);
        ui.kosikPridat->setEnabled(true);
        ui.kosikOdobrat->setEnabled(true);
        ui.Vytlacit->setEnabled(true);
        ui.cenyGroup->setVisible(true);

        if (Pouzivatelia[ui.vyberPouzivatela->currentIndex()]->GetVybaveny()) {       //obrati stav vybavenosti
            ui.Pripravene->setText("Označiť ako Nepripravené");
            ui.kosikPridat->setEnabled(false);                                  //ak je objednavka pripravena, nemoze ju nikto menit
            ui.kosikOdobrat->setEnabled(false);
            ui.LiekyKosik->setEnabled(false);
            ui.LiekyPredaj->setEnabled(false);
        }
        else {
            ui.Pripravene->setText("Označiť ako Pripravené");
            ui.kosikPridat->setEnabled(true);
            ui.kosikOdobrat->setEnabled(true);
            ui.LiekyKosik->setEnabled(true);
            ui.LiekyPredaj->setEnabled(true);
        }


        if (Pouzivatelia[ui.vyberPouzivatela->currentIndex()]->GetZlava() == 0.0) {      //testuje, ci zakaznik ma zlavu
            ui.LiekyPredaj->setColumnHidden(2, true);
            ui.LiekyKosik->setColumnHidden(2, true);       //ak nie, tak nezobrazuje stlpce
        }
        else {
            ui.LiekyPredaj->setColumnHidden(2, false);      
            ui.LiekyKosik->setColumnHidden(2, false);
        }
    }
    
    if (Pouzivatelia[ui.vyberPouzivatela->currentIndex()]->GetIndex() == 0) {        //nastavim vypis pri vybere pouzivatela
        if (Pouzivatelia[ui.vyberPouzivatela->currentIndex()]->GetZlava() != 0.0) {
            ui.poziciaVybrany->setText("Prémiový zákazník");
        }
        else {
            ui.poziciaVybrany->setText("Zákazník");
        } 
    }
    else if (Pouzivatelia[ui.vyberPouzivatela->currentIndex()]->GetIndex() == 1) {
        ui.poziciaVybrany->setText("Zamestnanec lekárne");
    }
    else {
        ui.poziciaVybrany->setText("Admin");
    }
    AktualizujPonuku();
    AktualizujKosik();
}

void lekaren::ZafarbiObchod() {
    int i;
    if (!VysledkyHladania.isEmpty()) {      //ak je v obchode zobrazene hocico, otestuje
        if (VysledkyHladania[0] != -1) {        //ak su v obchode zobrazene len vyhladane lieky
            for (i = 0; i < VysledkyHladania.size(); i++) {
                if(Pouzivatelia[ui.vyberPouzivatela->currentIndex()]->LiekyDataContains(VysledkyHladania[i])){   //v kosiku pouzivatela z "vyber zakaznika" hladam, ci obsahuje index z vysledkov hladania
                    ui.LiekyPredaj->item(i, 0)->setBackground(QColor(102, 255, 194));     //nastavim nakupene lieky na zeleno v obchode
                    ui.LiekyPredaj->item(i, 1)->setBackground(QColor(102, 255, 194));
                    ui.LiekyPredaj->item(i, 2)->setBackground(QColor(102, 255, 194));
                }
                else {
                    ui.LiekyPredaj->item(i, 0)->setBackground(Qt::white);
                    ui.LiekyPredaj->item(i, 1)->setBackground(Qt::white);
                    ui.LiekyPredaj->item(i, 2)->setBackground(Qt::white);
                }
            }
        }
        else {                                          //ak su v obchode zobrazene vsetky lieky
            for (i = 0; i < Lieky.size(); i++) {
                if (Pouzivatelia[ui.vyberPouzivatela->currentIndex()]->LiekyDataContains(i)) {
                    ui.LiekyPredaj->item(i, 0)->setBackground(QColor(102, 255, 194));     //nastavim nakupene lieky na zeleno v obchode
                    ui.LiekyPredaj->item(i, 1)->setBackground(QColor(102, 255, 194));
                    ui.LiekyPredaj->item(i, 2)->setBackground(QColor(102, 255, 194));
                }
                else {
                    ui.LiekyPredaj->item(i, 0)->setBackground(Qt::white);
                    ui.LiekyPredaj->item(i, 1)->setBackground(Qt::white);
                    ui.LiekyPredaj->item(i, 2)->setBackground(Qt::white);
                }
            }
        }
    }
}

void lekaren::AktualizujKosik()
{
    QString loginVybraneho = ui.vyberPouzivatela->currentText();
    int indexVybraneho = ui.vyberPouzivatela->currentIndex();
    int velkost;

    if (Pouzivatelia[indexVybraneho]->GetIndex() == 1 || Pouzivatelia[indexVybraneho]->GetIndex() == 2) {
        velkost = 0;
    }
    else {
        velkost = Pouzivatelia[indexVybraneho]->GetLiekyDataSize();
    }
        
    ui.LiekyKosik->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui.LiekyKosik->setRowCount(velkost);

    int i;
    if (velkost != 0) {

        for (i = 0; i < velkost; i++) {
            medicine Tliek = Lieky[Pouzivatelia[indexVybraneho]->GetLiekyDataIndex(i)];
            float zlava = 1.0 - (Pouzivatelia[indexVybraneho]->GetZlava() * 0.01);

            QTableWidgetItem* itemMeno = new QTableWidgetItem(Tliek.GetMeno());
            itemMeno->setFlags(itemMeno->flags() ^ Qt::ItemIsEditable);                                         //nastavim, aby neboli editable

            QTableWidgetItem* itemCena = new QTableWidgetItem(QString::number(Tliek.GetCena()));
            itemCena->setFlags(itemCena->flags() ^ Qt::ItemIsEditable);

            
            QTableWidgetItem* itemCenaZlava = new QTableWidgetItem(QString::number(Tliek.GetCena() * zlava, 'f', 2));
            itemCenaZlava->setFlags(itemCenaZlava->flags() ^ Qt::ItemIsEditable);

            ui.LiekyKosik->setItem(i, 0, itemMeno);
            ui.LiekyKosik->setItem(i, 1, itemCena); 
            ui.LiekyKosik->setItem(i, 2, itemCenaZlava);
        }
    }
    ZafarbiObchod();
    AktualizujCeny();
}

void lekaren::UploadLieky(QString nazov_suboru)
{
    VysledkyHladania.clear();
    VysledkyHladania = { -1 };

    QFile listliek(nazov_suboru);             //nacitaju sa lieky zo suboru
    if (!listliek.open(QIODevice::ReadOnly)) {      //kontrola spravneho otvorenia suboru
        QMessageBox msgBox;
        msgBox.setText("S�bor neexistuje.");
        msgBox.exec();
        return;
    }
    QString line = listliek.readLine();
    
    if (line.split(",").at(0) != "#databaza_liekov#") {      //kontrola formatu
        QMessageBox msgBox;
        msgBox.setText("Súbor je zapísaný v nesprávnom formáte.");
        msgBox.exec();
        return;
    }

    int i;
 
    while(!listliek.atEnd()){
        line = listliek.readLine();       //precita cely riadok csv

        QString Tmeno = (line.split(',').at(0));
        float Tcena = (line.split(',').at(1).toFloat());

        if (LiekyContains(Tmeno)) {        //ak liek uz je v zozname, prepise jeho cenu
            Lieky[LiekyIndexOf(Tmeno)].SetCena(Tcena);
        }
        else {                   //ak nie je v zozname, prida ho do zoznamu
            medicine Tliek(Tmeno, Tcena);
            Lieky.push_back(Tliek);
        }
    }
    listliek.close();

    if (nazov_suboru != "lieky.csv") {      //ak to nie je povodny subor, updatne povodny s novymi liekmi
        QFile listliek("lieky.csv");             //nacitaju sa lieky zo suboru
        if (!listliek.open(QIODevice::WriteOnly | QIODevice::Text)) {      //kontrola spravneho otvorenia suboru
            QMessageBox msgBox;
            msgBox.setText("Aktualizovaný zoznam sa nepodarilo uložiť.");
            msgBox.exec();
            return;
        }

        int i;
        QTextStream out(&listliek);
        out << "#databaza_liekov#,";

        for (i = 0; i < Lieky.size(); i++) {
            out << "\n" << Lieky[i].GetMeno() << "," << Lieky[i].GetCena() << ",";
        }
        listliek.close();

        AktualizujPonuku();
        AktualizujKosik();
    }
}

void lekaren::UploadPouz()
{
    Pouzivatelia.clear();

    QFile listpouz("pouzivatelia.csv");             //nacitaju sa loginy zo suboru
    if (!listpouz.open(QIODevice::ReadOnly)) {      //kontrola spravneho otvorenia suboru
        QMessageBox msgBox;
        msgBox.setText("Nepodaril sa otvoriť zoznam používateľov.");
        msgBox.exec();
        exit(1);
    }
    QString line = listpouz.readLine();
    if (line.split(',').at(0) == "#pouzivatelia#") {

        int i=0;
        while(!listpouz.atEnd()){
            QString line = listpouz.readLine();       //precita cely riadok csv

            int Tindex = (line.split(',').at(0).toInt());
            QString Tlogin = (line.split(',').at(1));
            QString Theslo = (line.split(',').at(2));
            QString Tmeno = (line.split(',').at(3));
            QString Tpriezvisko = (line.split(',').at(4));

            if (Tindex == 0) {      //zakaznik
                QString Tulica = (line.split(',').at(5));
                QString Tmesto = (line.split(',').at(6));
                QString Tpsc = (line.split(',').at(7));
                float Tminute = (line.split(',').at(8).toFloat());
                bool Tvybaveny;
                if (line.split(',').at(9) == "0") Tvybaveny = false;
                else Tvybaveny = true;

                Pouzivatelia.append(new zakaznik(Tindex, Tlogin, Theslo, Tmeno, Tpriezvisko, Tulica, Tmesto, Tpsc, Tminute, Tvybaveny));
          
            }
            else if (Tindex == 1) {     //zamestnanec
                QString Tpozicia = (line.split(',').at(5));
                Pouzivatelia.append(new zamestnanec(Tindex, Tlogin, Theslo, Tmeno, Tpriezvisko, Tpozicia));

            }
            else if (Tindex == 2){      //admin
                Pouzivatelia.append(new admin(Tindex, Tlogin, Theslo, Tmeno, Tpriezvisko));
            }
        }
    }
    else {
        QMessageBox msgBox;
        msgBox.setText("Nájdený súbor je neplatného formátu.");
        msgBox.exec();
        listpouz.close();
        exit(1);
    }
    listpouz.close();
}

void lekaren::UploadKosiky()
{
    QFile pouzkosiky("pouzkosiky.csv");
    if (!pouzkosiky.open(QIODevice::ReadOnly)) {      //kontrola spravneho otvorenia suboru
        QMessageBox msgBox;
        msgBox.setText("Nepodaril sa otvoriť zoznam košíkov používateľov.");
        msgBox.exec();
        return;
    }
    QString line = pouzkosiky.readLine();

    if (line.split(",").at(0) != "#databaza_kosikov_pouzivatelov#") {
        QMessageBox msgBox;
        msgBox.setText("Súbor bol uložený v neplatnom formáte.");
        msgBox.exec();
        pouzkosiky.close();
        return;
    }
    else {
        while (!pouzkosiky.atEnd()) {
            QString line = pouzkosiky.readLine();
            int indexPouz = line.split(",").at(0).toInt();
            QVector<QString> temp = line.split(",").toVector();
            temp.removeFirst();
            temp.removeLast();
            Pouzivatelia[indexPouz]->SetLiekyData(ConvertToInt(&temp));
        }
        pouzkosiky.close();
    }
}

void lekaren::AktualizujPonuku() {
    int i;

    if (VysledkyHladania.size() != 0) {      //ak je nulove, nenapise nic, inak pise vysledky

        ui.LiekyPredaj->setRowCount(VysledkyHladania.size());
        ui.LiekyPredaj->setSelectionBehavior(QAbstractItemView::SelectRows);

        if (VysledkyHladania[0] == -1) {      //napise cely zoznam liekov

            ui.LiekyPredaj->setRowCount(Lieky.size());

            for (i = 0; i < Lieky.size(); i++) {    
                QTableWidgetItem* itemMeno = new QTableWidgetItem(Lieky[i].GetMeno());
                itemMeno->setFlags(itemMeno->flags() ^ Qt::ItemIsEditable);             //nastavim, aby neboli editable

                QTableWidgetItem* itemCena = new QTableWidgetItem(QString::number(Lieky[i].GetCena()));
                itemCena->setFlags(itemCena->flags() ^ Qt::ItemIsEditable);             //nastavim, aby neboli editable

                float zlava = 1.0;
                if (ui.vyberPouzivatela->count() != 0) {
                    zlava = 1.0 - (Pouzivatelia[ui.vyberPouzivatela->currentIndex()]->GetZlava() * 0.01);
                }
                
                QTableWidgetItem* itemCenaZlava = new QTableWidgetItem(QString::number(Lieky[i].GetCena() * zlava,'f',2));
                itemCenaZlava->setFlags(itemCenaZlava->flags() ^ Qt::ItemIsEditable);
                
                ui.LiekyPredaj->setItem(i, 0, itemMeno);
                ui.LiekyPredaj->setItem(i, 1, itemCena);
                ui.LiekyPredaj->setItem(i, 2, itemCenaZlava);
            }
        }

        else {      //napise iba vyhovujuce vysledky
            for (i = 0; i < VysledkyHladania.size(); i++) {
                QTableWidgetItem* itemMeno = new QTableWidgetItem(Lieky[VysledkyHladania[i]].GetMeno());
                itemMeno->setFlags(itemMeno->flags() ^ Qt::ItemIsEditable);                          //nastavim, aby neboli editable

                QTableWidgetItem* itemCena = new QTableWidgetItem(QString::number(Lieky[VysledkyHladania[i]].GetCena()));
                itemCena->setFlags(itemCena->flags() ^ Qt::ItemIsEditable);

                float zlava = 1.0;

                if (ui.vyberPouzivatela->count() != 0) {
                    zlava = 1.0 - (Pouzivatelia[ui.vyberPouzivatela->currentIndex()]->GetZlava() * 0.01);
                }

                QTableWidgetItem* itemCenaZlava = new QTableWidgetItem(QString::number(Lieky[VysledkyHladania[i]].GetCena() * zlava, 'f', 2));
                itemCenaZlava->setFlags(itemCenaZlava->flags() ^ Qt::ItemIsEditable);

                ui.LiekyPredaj->setItem(i, 0, itemMeno);
                ui.LiekyPredaj->setItem(i, 1, itemCena);
                ui.LiekyPredaj->setItem(i, 2, itemCenaZlava);
            }
        }
    }
    else {
        ui.LiekyPredaj->setRowCount(0);
    }
}

void lekaren::AktualizujCeny()
{
    int i;
    if (Pouzivatelia[ui.vyberPouzivatela->currentIndex()]->GetZlava() == 0.0) {
        ui.label_13->setVisible(false);
        ui.cenaPoZlave->setVisible(false);
    }
    else {
        ui.label_13->setVisible(true);
        ui.cenaPoZlave->setVisible(true);
    }
    float cena = 0;
    for(i=0; i < Pouzivatelia[ui.vyberPouzivatela->currentIndex()]->GetLiekyDataSize();i++){
        cena+= Lieky[Pouzivatelia[ui.vyberPouzivatela->currentIndex()]->GetLiekyDataIndex(i)].GetCena();
    }

    ui.cena->setValue(cena);
    ui.cenaPoZlave->setValue((1.0 - (Pouzivatelia[ui.vyberPouzivatela->currentIndex()]->GetZlava() * 0.01)) * cena);
}

void lekaren::DesignMain()
{
    setStyleSheet("background:rgb(250,250,250);");
    ui.groupBox->setStyleSheet("background:rgb(179, 255, 224);");     //mint
    ui.groupBox_2->setStyleSheet("background:rgb(179, 255, 224);");
    ui.LiekyPredaj->setStyleSheet("background:rgb(250, 250, 250);");      //biela
    ui.LiekyKosik->setStyleSheet("background:rgb( 250, 250, 250);");
    ui.kosikPridat->setStyleSheet("background:rgb( 250, 250, 250);");
    ui.kosikOdobrat->setStyleSheet("background:rgb( 250, 250, 250);");
    ui.Vytlacit->setStyleSheet("background:rgb( 250, 250, 250);");
    ui.Pripravene->setStyleSheet("background:rgb( 250, 250, 250);");
    ui.HladatLiek->setStyleSheet("background:rgb( 250, 250, 250);");
    ui.vyberPouzivatela->setStyleSheet("background:rgb( 250, 250, 250);");
    ui.cena->setStyleSheet("background:rgb( 250, 250, 250);");
    ui.cenaPoZlave->setStyleSheet("background:rgb( 250, 250, 250);");
    ui.poziciaVybrany->setStyleSheet("background:rgb( 250, 250, 250);");

    ui.meno->setStyleSheet("background:rgb( 250, 250, 250);");
    ui.priezvisko->setStyleSheet("background:rgb( 250, 250, 250);");
    ui.ulica->setStyleSheet("background:rgb( 250, 250, 250);");
    ui.mesto->setStyleSheet("background:rgb( 250, 250, 250);");
    ui.psc->setStyleSheet("background:rgb( 250, 250, 250);");
    ui.pozicia->setStyleSheet("background:rgb( 250, 250, 250);");
    ui.zlava->setStyleSheet("background:rgb( 250, 250, 250);");

    ui.groupBox_3->setStyleSheet("background:rgb(179, 255, 224);");
    ui.UploadZoznam->setStyleSheet("background:rgb(250, 250, 250);");
    ui.UprPouz->setStyleSheet("background:rgb(250, 250, 250);");
    ui.NovyPouz->setStyleSheet("background:rgb(250, 250, 250);");
    ui.Odhlasit->setStyleSheet("background:rgb(250, 250, 250);"); 
}

void lekaren::ZmenaObsahuTabulky() 
{
    int Tchyba = 0;             //docasna premenna na kontrolu chybneho vstupu
    int riadok = DIALOGpouz->GetCurrentRow();
    QTableWidgetItem* bunka = DIALOGpouz->GetCurrentItem();
    QString obsahBunky = DIALOGpouz->GetCurrentItemText();
    int indexPouz = DIALOGpouz->GetItemText(riadok, 0).toInt();

    if (KontrolaTextu(obsahBunky)) {
        QMessageBox msgBox;
        msgBox.setText("Použitý zakázaný symbol ',' alebo '\\' .");
        msgBox.exec();

        Tchyba = 1;
    }
    if (obsahBunky.isEmpty()) {
        Tchyba = 1;
    }


    switch (DIALOGpouz->GetCurrentColumn()) {


    case 0:     //index
        if (DIALOGpouz->GetCurrentColumn() == 0) {      //davam obvious podmienky, inak by sa kompilator stazoval na staryObsah
            QMessageBox msgBox;
            msgBox.setText("Index používateľa nie je možné meniť.");
            msgBox.exec();

            QString staryObsah = QString::number(Pouzivatelia[riadok]->GetIndex());
            DIALOGpouz->BlockSignals(true);
            DIALOGpouz->SetCurrentItemText(staryObsah);
            DIALOGpouz->BlockSignals(false);

            break;
        }

    case 1:     //login

        if (Tchyba == 1) {
            QString staryObsah = Pouzivatelia[riadok]->GetLogin();
            DIALOGpouz->BlockSignals(true);
            DIALOGpouz->SetCurrentItemText(staryObsah);
            DIALOGpouz->BlockSignals(false);
        }
        else {
            Pouzivatelia[riadok]->SetLogin(obsahBunky);
            SetZmeny(10, 2);
        }
        break;

    case 2:     //heslo
        if (Tchyba == 1) {
            DIALOGpouz->BlockSignals(true);
            DIALOGpouz->SetCurrentItemText("********");
            DIALOGpouz->BlockSignals(false);
        }
        else {
            Pouzivatelia[riadok]->SetHeslo(PswdEnc(obsahBunky));
            DIALOGpouz->BlockSignals(true);
            DIALOGpouz->SetCurrentItemText("********");
            DIALOGpouz->BlockSignals(false);
            SetZmeny(11, 2);
        }
        break;
    case 3:     //meno

        if (Tchyba == 1) {
            QString staryObsah = Pouzivatelia[riadok]->GetMeno();
            DIALOGpouz->BlockSignals(true);
            DIALOGpouz->SetCurrentItemText(staryObsah);
            DIALOGpouz->BlockSignals(false);
        }
        else {
            Pouzivatelia[riadok]->SetMeno(obsahBunky);

            if (DIALOGpouz->GetItemText(riadok, 1) == Prihlaseny->GetLogin()) {
                ui.meno->setText(Prihlaseny->GetMeno());
            }
            SetZmeny(0, 2);
        }
        break;
    case 4:     //priezvisko
        if (Tchyba == 1) {
            QString staryObsah = Pouzivatelia[riadok]->GetPriezvisko();
            DIALOGpouz->BlockSignals(true);
            DIALOGpouz->SetCurrentItemText(staryObsah);
            DIALOGpouz->BlockSignals(false);
        }
        else {
            Pouzivatelia[riadok]->SetPriezvisko(obsahBunky);

            if (DIALOGpouz->GetItemText(riadok, 1) == Prihlaseny->GetLogin()) {
                ui.priezvisko->setText(Prihlaseny->GetPriezvisko());
            }
            SetZmeny(1, 2);
        }
        break;
    case 5:     //ulica
        if (indexPouz != 0) {               //ak to nie je zakaznik, len vrati zablokovanu bunku
            DIALOGpouz->BlockSignals(true);
            DIALOGpouz->SetCurrentItemText("--");
            DIALOGpouz->BlockSignals(false);
        }
        else {
            if (Tchyba == 1) {
                QString staryObsah = Pouzivatelia[riadok]->GetUlica();
                DIALOGpouz->BlockSignals(true);
                DIALOGpouz->SetCurrentItemText(staryObsah);
                DIALOGpouz->BlockSignals(false);
            }
            else {
                Pouzivatelia[riadok]->SetUlica(obsahBunky);
                SetZmeny(2, 2);
            }
        }
        break;
    case 6:     //mesto
        if (indexPouz != 0) {               //ak to nie je zakaznik, len vrati zablokovanu bunku
            DIALOGpouz->BlockSignals(true);
            DIALOGpouz->SetCurrentItemText("--");
            DIALOGpouz->BlockSignals(false);
        }
        else {
            if (Tchyba == 1) {
                QString staryObsah = Pouzivatelia[riadok]->GetMesto();
                DIALOGpouz->BlockSignals(true);
                DIALOGpouz->SetCurrentItemText(staryObsah);
                DIALOGpouz->BlockSignals(false);
            }
            else {
                Pouzivatelia[riadok]->SetMesto(obsahBunky);
                SetZmeny(3, 2);
            }
        }
        break;
    case 7:     //psc
        if (indexPouz != 0) {       //ak to nie je zakaznik, len vrati zablokovanu bunku
            DIALOGpouz->BlockSignals(true);
            DIALOGpouz->SetCurrentItemText("--");
            DIALOGpouz->BlockSignals(false);
        }
        else {
            if (Tchyba == 1) {
                QString staryObsah = Pouzivatelia[riadok]->GetPsc();
                DIALOGpouz->BlockSignals(true);
                DIALOGpouz->SetCurrentItemText(staryObsah);
                DIALOGpouz->BlockSignals(false);
            }
            else {
                try {       //kontrola neciselneho vstupu
                    obsahBunky.toInt();     //aj ked to neukladam ako cislo, ale takto overim, ci to je cislo
                    Pouzivatelia[riadok]->SetPsc(obsahBunky);
                    SetZmeny(4, 2);
                }
                catch (...) {       //ak zachyti hociaku chybu, postazuje sa a vrati staru hodnotu
                    QMessageBox msgBox;
                    msgBox.setText("Neplatné PSČ.");
                    msgBox.exec();
                    QString staryObsah = Pouzivatelia[riadok]->GetPsc();
                    DIALOGpouz->BlockSignals(true);
                    DIALOGpouz->SetCurrentItemText(staryObsah);
                    DIALOGpouz->BlockSignals(false);
                }
            }
        }

        break;
    case 8:     //minute
        if (indexPouz != 0) {       //ak to nie je zakaznik, len vrati zablokovanu bunku
            DIALOGpouz->BlockSignals(true);
            DIALOGpouz->SetCurrentItemText("--");
            DIALOGpouz->BlockSignals(false);
        }
        else {
            if (Tchyba == 1) {
                QString staryObsah = QString::number(Pouzivatelia[riadok]->GetMinute());
                DIALOGpouz->BlockSignals(true);
                DIALOGpouz->SetCurrentItemText(staryObsah);
                DIALOGpouz->BlockSignals(false);
            }
            else {
                try {                       //kontrola neciselneho vstupu

                    Pouzivatelia[riadok]->SetMinute(obsahBunky.toFloat());
                    SetZmeny(12, 2);
                    Pouzivatelia[riadok]->SetZlava();
                }
                catch (...) {       //ak zachyti hociaku chybu, postazuje sa a vrati staru hodnotu
                    QMessageBox msgBox;
                    msgBox.setText("Neplatný vstup.");
                    msgBox.exec();
                    QString staryObsah = QString::number(Pouzivatelia[riadok]->GetMinute());
                    DIALOGpouz->BlockSignals(true);
                    DIALOGpouz->SetCurrentItemText(staryObsah);
                    DIALOGpouz->BlockSignals(false);
                }
            }
        }
        break;
    case 9:     //pozicia
        if (indexPouz != 1) {       //ak to nie je pracovnik, len vrati zablokovanu bunku
            DIALOGpouz->BlockSignals(true);
            DIALOGpouz->SetCurrentItemText("--");
            DIALOGpouz->BlockSignals(false);
        }
        else {
            if (Tchyba == 1) {
                QString staryObsah = Pouzivatelia[riadok]->GetPozicia();
                DIALOGpouz->BlockSignals(true);
                DIALOGpouz->SetCurrentItemText(staryObsah);
                DIALOGpouz->BlockSignals(false);
            }
            else {
                Pouzivatelia[riadok]->SetPozicia(obsahBunky);
                SetZmeny(5, 2);
                ui.pozicia->setText(obsahBunky);
            }
        }
        break;
    case 10:        //vybavene
        if (indexPouz != 0) {       //ak to nie je zakaznik, len vrati zablokovanu bunku
            DIALOGpouz->BlockSignals(true);
            DIALOGpouz->SetCurrentItemText("--");
            DIALOGpouz->BlockSignals(false);
        }
        else {
            if (Tchyba == 1 || (obsahBunky != "0" && obsahBunky != "1")) {

                if (obsahBunky != "0" && obsahBunky != "1") {
                    QMessageBox msgBox;
                    msgBox.setText("Neplatný index. Zadajte číslo 0 alebo 1.");
                    msgBox.exec();
                }
                QString staryObsah = "2";
                if (Pouzivatelia[riadok]->GetVybaveny()) {
                    staryObsah = "1";
                }
                else {
                    staryObsah = "0";
                }
                DIALOGpouz->BlockSignals(true);
                DIALOGpouz->SetCurrentItemText(staryObsah);
                DIALOGpouz->BlockSignals(false);
            }
            else {
                if (obsahBunky == "1") {
                    Pouzivatelia[riadok]->SetVybaveny(true);

                    ui.Pripravene->setText("Označiť ako Nepripravené");
                    ui.kosikPridat->setEnabled(false);                      //ak je objednavka pripravena, nemoze sa menit
                    ui.kosikOdobrat->setEnabled(false);

                    ui.LiekyKosik->setEnabled(false);
                    ui.LiekyPredaj->setEnabled(false);

                }
                else {
                    Pouzivatelia[riadok]->SetVybaveny(false);

                    ui.Pripravene->setText("Označiť ako Pripravené");
                    ui.kosikPridat->setEnabled(true);
                    ui.kosikOdobrat->setEnabled(true);

                    ui.LiekyKosik->setEnabled(true);
                    ui.LiekyPredaj->setEnabled(true);
                }
                SetZmeny(8, 2);
            }
        }
        break;

    default:

        break;
    }

}

void lekaren::OdstranenyPouz()
{
    int riadok = DIALOGpouz->GetCurrentRow();
    Pouzivatelia.removeAt(riadok);
    DIALOGpouz->RemoveRow(riadok);
    ui.vyberPouzivatela->removeItem(riadok);
    SetZmeny(10, 2);     //nastavim, aby ocakavalo zmenu medzi pouzivatelmi a teda prepisalo zoznam pouzivatelov
    SetZmeny(6, 3);      //prepise aj zoznam kosikov
}

void lekaren::TabPouzZmenaRiadku()
{
    int riadok = DIALOGpouz->GetCurrentRow();
    if (PouzivateliaIndexOfLogin(Prihlaseny->GetLogin()) == riadok) {       //ak je na vlastnom riadku
        DIALOGpouz->EnableOdstr(false);
    }
    else {
        DIALOGpouz->EnableOdstr(true);
    }
}


void NovyPouzDialog::on_index_currentIndexChanged(int)
{
    if (ui->index->currentIndex() == 1) {		//ak to je zamestnanec
        SetKontrola(7, 1);
        ui->label_14->setVisible(true);
        ui->pozicia->setVisible(true);

        ui->label_5->setVisible(false);
        ui->vybaveny->setVisible(false);
 
        ui->label_9->setVisible(false);
        ui->minute->setVisible(false);

        ui->notAdmin->setVisible(true);
    }
    else if (ui->index->currentIndex() == 2) {		//ak to je admin
        ui->notAdmin->setVisible(false);
        ui->ulica->setText("");
        ui->mesto->setText("");
        ui->psc->setText("");
        ui->minute->setValue(0.0);
        ui->pozicia->setText("");
        SetKontrola(4, 0);
        SetKontrola(5, 0);
        SetKontrola(6, 0);
        SetKontrola(7, 0);
    }
    else {											//ak to je zakaznik
        ui->pozicia->setVisible(false);		//skryje pracovnu poziciu, ak to nie je zamestnanec
        ui->label_14->setVisible(false);

        ui->vybaveny->setVisible(true);		//zobrazi vybavenost iba ak to je zakaznik
        ui->label_5->setVisible(true);

        ui->notAdmin->setVisible(true);

        ui->pozicia->setText("");
        SetKontrola(7, 0);
    }
    CheckChyby();
}

void NovyPouzDialog::on_login_textChanged()
{
    if (ui->login->text() == "")  SetKontrola(0, 1);
    else SetKontrola(0, 0);
    CheckChyby();
}

void NovyPouzDialog::on_heslo_textChanged()
{
    if (ui->heslo->text() == "")  SetKontrola(1, 1);
    else SetKontrola(1, 0);
    CheckChyby();
}

void NovyPouzDialog::on_meno_textChanged()
{
    if (ui->meno->text() == "")  SetKontrola(2,1);
    else SetKontrola(2, 0);
    CheckChyby();
}

void NovyPouzDialog::on_priezvisko_textChanged()
{
    if (ui->priezvisko->text() == "")  SetKontrola(3, 1);
    else SetKontrola(3, 0);
    CheckChyby();
}

void NovyPouzDialog::on_ulica_textChanged()
{
    if (ui->ulica->text() == "")  SetKontrola(4, 1);
    else SetKontrola(4, 0);
    CheckChyby();
}

void NovyPouzDialog::on_mesto_textChanged()
{
    if (ui->mesto->text() == "")  SetKontrola(5, 1);
    else SetKontrola(5, 0);
    CheckChyby();
}

void NovyPouzDialog::on_psc_textChanged()
{
    if (ui->psc->text() == "")  SetKontrola(6, 1);
    else SetKontrola(6, 0);
    CheckChyby();
}

void NovyPouzDialog::on_pozicia_textChanged()
{
    if (ui->pozicia->text() == "")  SetKontrola(7, 1);
    else SetKontrola(7, 0);
    CheckChyby();
}

void NovyPouzDialog::on_zobrazHeslo_clicked()
{
    if (ui->zobrazHeslo->isChecked()) ui->heslo->setEchoMode(QLineEdit::Normal);
    else  ui->heslo->setEchoMode(QLineEdit::Password);
}

void NovyPouzDialog::CheckChyby()
{
    if (KontrolaChyby.contains(1)) ui->OkButton->setEnabled(false);
    else ui->OkButton->setEnabled(true);
}


/////////////////////////////////////////////////////////


user::user()
{
    //qDebug() << "konstruktor prazdny user";
    UserIndex = 0;
    UserLogin = "";
    UserHeslo = "";
    UserMeno = "NA";
    UserPriezvisko = "NA";  
}

user::user(int in, QString nn, QString pss, QString mn, QString pr)
{
    //qDebug() << "konstruktor plny user";
    UserIndex = in;
    UserLogin = nn;
    UserHeslo = pss;
    UserMeno = mn;
    UserPriezvisko = pr;
}

zakaznik::zakaznik(int in, QString nn, QString pss, QString mn, QString pr, QString ul, QString ms, QString ps, float min, bool vyb) : user(in, nn, pss, mn, pr)
{
    //qDebug() << "konstruktor plny zakaznik";
    UserUlica = ul;
    UserMesto = ms;
    UserPsc = ps;
    UserMinute = min;
    UserVybaveny = vyb;
    
    if (UserMinute >= 100) {
        if ((UserZlava = floor(UserMinute / 100)) > 20) UserZlava = 20.0;
    }
    else {
        UserZlava = 0.0;
    }
}

zakaznik::zakaznik() : user()
{
    //qDebug() << "konstruktor prazdny zakaznik";
    UserUlica = "NA";
    UserMesto = "NA";
    UserPsc = "NA";
    UserMinute = 0;
    UserVybaveny = false;
    UserZlava = 0.0;
}

zamestnanec::zamestnanec(int in, QString nn, QString pss, QString mn, QString pr, QString poz) : user(in, nn, pss, mn, pr) {
    //qDebug() << "konstruktor plny zamestnanec";
    UserPozicia = poz;
}

zamestnanec::zamestnanec() : user() {
    //qDebug() << "konstruktor prazdny zamestnanec";
    UserPozicia = "NA";
}

admin::admin(int in, QString nn, QString pss, QString mn, QString pr) : user(in, nn, pss, mn, pr) {
    qDebug() << "konstruktor plny admin";
}

admin::admin() : user() {
    qDebug() << "konstruktor prazdny admin";
}