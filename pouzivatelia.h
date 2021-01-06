#pragma once
/*
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
    void SetZlava() { if ((UserZlava = UserMinute / 100) > 20) UserZlava = 20; };
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
    QString GetPozicia() { return UserPozicia; };
    void SetPozicia(QString input) { UserPozicia = input; };
};

class admin : public user {
private:
public:
    admin::admin();
    admin::admin(int in, QString nn, QString pss, QString mn, QString pr);
};


user::user()
{
    UserIndex = 0;
    UserLogin = "";
    UserHeslo = "";
    UserMeno = "NA";
    UserPriezvisko = "NA";
}

user::user(int in, QString nn, QString pss, QString mn, QString pr)
{
    UserIndex = in;
    UserLogin = nn;
    UserHeslo = pss;
    UserMeno = mn;
    UserPriezvisko = pr;
}

zakaznik::zakaznik(int in, QString nn, QString pss, QString mn, QString pr, QString ul, QString ms, QString ps, float min, bool vyb): user(in, nn, pss, mn, pr)
{
    UserUlica = ul;
    UserMesto = ms;
    UserPsc = ps;
    UserMinute = min;
    UserVybaveny = vyb;

    if ((UserZlava = UserMinute / 100) > 20) UserZlava = 20;
}

zakaznik::zakaznik() : user()
{
    UserUlica = "NA";
    UserMesto = "NA";
    UserPsc = "NA";
    UserMinute = 0;
    UserVybaveny = false;

    if ((UserZlava = UserMinute / 100) > 20) UserZlava = 20;
}

zamestnanec::zamestnanec(int in, QString nn, QString pss, QString mn, QString pr, QString poz) : user(in, nn, pss, mn, pr) {
    UserPozicia = poz;
}

zamestnanec::zamestnanec() : user() {
    UserPozicia = "NA";
}

admin::admin(int in, QString nn, QString pss, QString mn, QString pr) : user(in, nn, pss, mn, pr) {

}

admin::admin() : user() {

}
*/