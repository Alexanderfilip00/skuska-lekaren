#pragma once

#include <QtWidgets>
#include "ui_lekaren.h"
#include "NovyPouzDialog.h"
#include "LoginDialog.h"
#include "TabulkaPouz.h"

class user
{
private:
    int UserIndex;
    QString UserLogin;
    QString UserHeslo;
    QString UserMeno;
    QString UserPriezvisko;

public:
    user();
    user(int in, QString nn, QString pss, QString mn, QString pr);
    virtual ~user() { 
        //qDebug() << "destruktor user"; 
    };

    int GetIndex() { return UserIndex; };
    QString GetLogin() { return UserLogin; };
    QString GetHeslo() { return UserHeslo; };
    QString GetMeno() { return UserMeno; };
    QString GetPriezvisko() { return UserPriezvisko; };

    void SetIndex(int input) { UserIndex = input; };
    void SetLogin(QString input) { UserLogin = input; };
    void SetHeslo(QString input) { UserHeslo = input; };
    void SetMeno(QString input) { UserMeno = input; };
    void SetPriezvisko(QString input) { UserPriezvisko = input; };


    //zakaznikove virtual funkcie
    virtual QString GetUlica() { return "NA"; };
    virtual QString GetMesto() { return "NA"; };
    virtual QString GetPsc() { return "NA"; };
    virtual float GetMinute() { return -1.0; };
    virtual float GetZlava() { return -1.0; };
    virtual bool GetVybaveny() { return false; };

    virtual void SetUlica(QString input) {};
    virtual void SetMesto(QString input) {};
    virtual void SetPsc(QString input) {};
    virtual void SetMinute(float input) {};
    virtual void SetZlava() {};
    virtual void SetVybaveny(bool input) {};

    virtual void SetLiekyData(QVector<int> LD) {};
    virtual int GetLiekyDataSize() { return -1; };
    virtual int GetLiekyDataIndex(int position) { return -1; };
    virtual bool LiekyDataContains(int index) { return false; };
    virtual void LiekyDataAppend(int index) {};
    virtual void LiekyDataRemove(int index) {};


    //zamestnanec
    virtual QString GetPozicia() { return "NA"; };
    virtual void SetPozicia(QString input) {};

};

class zakaznik : public user {
private:
    QString UserUlica;
    QString UserMesto;
    QString UserPsc;
    float UserZlava;
    float UserMinute;
    bool UserVybaveny;

    QVector<int> LiekyData;
public:
    zakaznik(int in, QString nn, QString pss, QString mn, QString pr, QString ul, QString ms, QString ps, float min, bool vyb);
    zakaznik();
    ~zakaznik() { 
        //qDebug() << "destruktor zakaznik"; 
    };

    QString GetUlica() { return UserUlica; };
    QString GetMesto() { return UserMesto; };
    QString GetPsc() { return UserPsc; };
    float GetMinute() { return UserMinute; };
    float GetZlava() { return UserZlava; };
    bool GetVybaveny() { return UserVybaveny; };

    void SetUlica(QString input) { UserUlica = input; };
    void SetMesto(QString input) { UserMesto = input; };
    void SetPsc(QString input) { UserPsc = input; };
    void SetMinute(float input) { UserMinute = input; };
    void SetZlava() { 
        if (UserMinute >= 100) {
            if ((UserZlava = floor(UserMinute / 100)) > 20) UserZlava = 20.0;
        }
        else {
            UserZlava = 0.0;
        }
    };
    void SetVybaveny(bool input) { UserVybaveny = input; };

    void SetLiekyData(QVector<int> LD) { LiekyData = LD; };
    int GetLiekyDataSize() { return LiekyData.size(); };
    int GetLiekyDataIndex(int position) { return LiekyData[position]; };
    bool LiekyDataContains(int index) { return LiekyData.contains(index); };
    void LiekyDataAppend(int index) { LiekyData.append(index); };
    void LiekyDataRemove(int index) { LiekyData.removeOne(index); };
};

class zamestnanec : public user {
private:
    QString UserPozicia;
public:
    zamestnanec(int in, QString nn, QString pss, QString mn, QString pr, QString poz);
    zamestnanec();
    ~zamestnanec() { 
        //qDebug() << "destruktor zamestnanec"; 
    };
    QString GetPozicia() { return UserPozicia; };
    void SetPozicia(QString input) { UserPozicia = input; };
};

class admin : public user {
private:
public:
    admin();
    admin(int in, QString nn, QString pss, QString mn, QString pr);
    ~admin() { 
        //qDebug() << "destruktor admin"; 
    };
};




class medicine {
private:
    QString meno;
    float cena;
public:
    medicine() { meno = "NA"; cena = 0.0; };
    medicine(QString m, float c) : meno(m), cena(c) {};

    QString GetMeno() { return meno; };
    float GetCena() { return cena; };
    void SetCena(float c) { cena = c; };
};


class lekaren : public QMainWindow
{
    Q_OBJECT

public:
    lekaren(QWidget *parent = Q_NULLPTR);
    ~lekaren();

    QString PswdEnc(QString heslo);
    QVector<int> ConvertToInt(QVector<QString>* input);

    int GetZmeny(int index) { return KontrolaZmien[index]; };
    void SetZmeny(int index, int value) { KontrolaZmien[index] = value; };
    bool CheckZmeny(int value) { return KontrolaZmien.contains(value); };
    bool LiekyContains(QString meno);
    int LiekyIndexOf(QString meno);

    int PouzivateliaIndexOfLogin(QString login);
    bool CheckHesloAt(int index, QString input);

private:
    Ui::lekarenClass ui;

    QVector<user *> Pouzivatelia;

    QVector<int> VysledkyHladania = {-1};       //na zaciatku taka hodnota, aby sa ukazala cela tabulka liekov v obchode
    QVector<int> KontrolaZmien = {0,0,0,0,0,0,0,0,0,0,0,0,0};
   
    QVector<medicine> Lieky;
    user* Prihlaseny;

    NovyPouzDialog* DIALOGnovyPouzivatel=nullptr;
    LoginDialog* LoginWindow=nullptr;
    TabulkaPouz* DIALOGpouz=nullptr;

    void PrisposobUI();
    bool KontrolaTextu(QString input);
    void closeEvent(QCloseEvent* event);

private slots:
    void on_NovyPouz_pressed();
    void NovyPouzPridany();
    void KontrolaLogin();
    void Exit();
    void on_Odhlasit_clicked();
    void check_Odhlasit();
    void on_UprPouz_clicked();
    void on_Vytlacit_clicked();
    void on_UploadZoznam_clicked();
 
    void on_meno_textChanged();
    void on_priezvisko_textChanged();
    void on_ulica_textChanged();
    void on_mesto_textChanged();
    void on_psc_textChanged();
    void on_pozicia_textChanged();

    void on_HladatLiek_textChanged();

    void on_kosikPridat_clicked();
    void on_LiekyPredaj_currentCellChanged();
    void on_kosikOdobrat_clicked();
    void on_LiekyKosik_currentCellChanged();
    void on_vyberPouzivatela_currentIndexChanged(int);
    void on_Pripravene_clicked();

    void ZafarbiObchod();
    void AktualizujKosik();
    void UploadLieky(QString nazov_suboru);
    void UploadPouz();
    void UploadKosiky();
    void AktualizujPonuku();
    void AktualizujCeny();

    void DesignMain();

public slots:
    void ZatvorDialogPouz() { DIALOGpouz->close(); };
    void ZmenaObsahuTabulky();
    void OdstranenyPouz();
    void TabPouzZmenaRiadku();
};
