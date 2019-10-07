#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QStringList>
#include <QMultiMap>
#include <QList>

class Grammar
{
public:
    Grammar(QByteArray input);
    Grammar(){};

    void load(QByteArray input);

    QJsonDocument get_Json() const;

    bool getIsValid() const;

private:
    QJsonDocument input;
    QJsonDocument output;
    bool isValid;
    bool check_valid();
    void write_info();
    struct symbol{
        QString name;
        bool type;
        bool operator ==(const symbol& value){
            if(value.name == name && value.type == type)
                return true;
            return false;
        }

    };

    QJsonObject terminalsymbolsJSON;
    QJsonObject nonterminalsymbolsJSON;
    QJsonObject startSymbolJSON;
    QJsonObject productionJSON;
    QStringList terminalsymbolsname;
    QStringList terminalsymbolsspell;
    QStringList nonterminalsymbolsname;
    QMultiMap<QString,QList<symbol>> production;
    QString startSymbol;
    void debug_print();
    void elimination_of_left_recursion();
    void make_output();
    void remove_useless_characters();
};

#endif // GRAMMAR_H
