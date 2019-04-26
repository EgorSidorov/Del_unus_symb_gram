#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QStringList>
#include <QMap>
#include <QList>

class Grammar
{
public:
    Grammar(QByteArray input);
    Grammar(){};

    load(QByteArray input);

    QJsonDocument get_Json() const;

    bool getIsValid() const;

private:
    QJsonDocument input;
    bool isValid;
    bool check_valid();
    void write_info();
    struct symbol{
        QString name;
        bool type;
    };

    QJsonObject terminalsymbolsJSON;
    QJsonObject nonterminalsymbolsJSON;
    QJsonObject startSymbolJSON;
    QJsonObject productionJSON;
    QStringList terminalsymbolsname;
    QStringList terminalsymbolsspell;
    QStringList nonterminalsymbolsname;
    QList<std::pair<QString,QList<symbol>>> production;
    QString startSymbol;
    void debug_print();
};

#endif // GRAMMAR_H
