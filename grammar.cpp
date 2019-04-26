#include "grammar.h"
#include <QVariantList>
#include <QJsonArray>
#include <QDebug>

Grammar::Grammar(QByteArray input)
{
    load(input);
}

QJsonDocument Grammar::get_Json() const
{
    return input;
}

bool Grammar::check_valid(){
    if(!input.object().contains("grammar"))
        return false;
    if(!input.object().take("grammar").toObject().contains("terminalsymbols"))
        return false;
    if(!input.object().take("grammar").toObject().contains("nonterminalsymbols"))
        return false;
    if(!input.object().take("grammar").toObject().contains("productions"))
        return false;
    if(!input.object().take("grammar").toObject().contains("startsymbol"))
        return false;
    return true;
}

void Grammar::write_info(){
    QJsonArray jsonArray;
    terminalsymbolsJSON = input.object().take("grammar").toObject().take("terminalsymbols").toObject();
    nonterminalsymbolsJSON = input.object().take("grammar").toObject().take("nonterminalsymbols").toObject();
    startSymbolJSON = input.object().take("grammar").toObject().take("startsymbol").toObject();
    productionJSON = input.object().take("grammar").toObject().take("productions").toObject();

    jsonArray = terminalsymbolsJSON.take("term").toArray();
    if(terminalsymbolsJSON.take("term").isObject()){
        QJsonValue iter = terminalsymbolsJSON.take("term").toObject();
        terminalsymbolsname.append(iter.toObject().take("-name").toString());
        terminalsymbolsspell.append(iter.toObject().take("-spell").toString());
    } else {
        for(QJsonValue iter : jsonArray){
            terminalsymbolsname.append(iter.toObject().take("-name").toString());
            terminalsymbolsspell.append(iter.toObject().take("-spell").toString());
        }
    }

    jsonArray = nonterminalsymbolsJSON.take("nonterm").toArray();
    if(nonterminalsymbolsJSON.take("nonterm").isObject()){
        QJsonValue iter = nonterminalsymbolsJSON.take("nonterm").toObject();
        nonterminalsymbolsname.append(iter.toObject().take("-name").toString());
    } else {
        for(QJsonValue iter : jsonArray){
            nonterminalsymbolsname.append(iter.toObject().take("-name").toString());
        }
    }

    startSymbol = startSymbolJSON.take("-name").toString();

    jsonArray = productionJSON.take("production").toArray();
    for(QJsonValue iter : jsonArray){
        QList<symbol> rhs;
        if(iter.toObject().take("rhs").toObject().take("symbol").isObject()){
            QJsonValue iter2 = iter.toObject().take("rhs").toObject().take("symbol").toObject();
            symbol iter_symbol;
            iter_symbol.name = iter2.toObject().take("-name").toString();
            iter2.toObject().take("-type").toString() == "term" ? iter_symbol.type = true : iter_symbol.type = false;
            rhs.append(iter_symbol);
        } else {
            for(QJsonValue iter2 : iter.toObject().take("rhs").toObject().take("symbol").toArray()){
                symbol iter_symbol;
                iter_symbol.name = iter2.toObject().take("-name").toString();
                iter2.toObject().take("-type").toString() == "term" ? iter_symbol.type = true : iter_symbol.type = false;
                rhs.append(iter_symbol);
            }
        }
        QString lhs = iter.toObject().take("lhs").toObject().take("-name").toString();
        production.append(std::make_pair(lhs,rhs));
    }
    debug_print();
}

void Grammar::debug_print(){
    qDebug()<<terminalsymbolsname;
    qDebug()<<terminalsymbolsspell;
    qDebug()<<nonterminalsymbolsname;
    qDebug()<<startSymbol;
    for(auto& iter : production){
        qDebug()<<"lhs:"<<iter.first;
        qDebug()<<"rhs:";
        for(auto& iter2 : iter.second){
            qDebug()<<iter2.name<<","<<iter2.type;
        }
    }
}

bool Grammar::getIsValid() const
{
    return isValid;
}

Grammar::load(QByteArray input)
{
    this->input = QJsonDocument::fromJson(input);
    isValid = check_valid();
    if(isValid)
        write_info();
}

