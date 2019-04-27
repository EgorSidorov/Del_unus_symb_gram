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
    return output;
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
        if(production.find(lhs,rhs) == production.end())
            production.insert(lhs,rhs);
    }
}

void Grammar::remove_useless_characters(){
    //ищем нетерминалы с левой стороны без нетерминалов с правой стороны или где все с правой стороны входят в множество
    QStringList added_rules;
    bool has_choosed = true;
    while(has_choosed){
        has_choosed = false;
        QStringList last_keys;
        for(auto& iter : production.keys()){
            if(last_keys.contains(iter))
                continue;
            for(auto& iter2 : production.values(iter)){
                bool isAdded = true;
                for(symbol& iter3 : iter2){
                    if(iter3.type){
                        for(auto& iter4 : iter2){
                            if(!iter4.type && !added_rules.contains(iter4.name)){
                                isAdded = false;
                                break;
                            }
                        }
                    }
                }
                if(isAdded && !added_rules.contains(iter)){
                    added_rules.append(iter);
                    has_choosed = true;
                }


            }
            last_keys.append(iter);
        }
    }
    qDebug()<<added_rules;

    //удалим непорождающие терминалы
    QStringList last_keys;
    for(auto& iter : production.keys()){
        if(last_keys.contains(iter))
            continue;
        bool isRemovedByName = false;
        if(!added_rules.contains(iter))
            isRemovedByName = true;
        for(auto& iter2 : production.values(iter)){
            bool isRemoved = false;
            for(symbol& iter3 : iter2){
                if(!added_rules.contains(iter3.name) && !iter3.type)
                {
                    isRemoved = true;
                    break;
                }
            }
            if(isRemoved || isRemovedByName)
                production.remove(iter,iter2);
        }
        last_keys.append(iter);
    }

    //QList<int> number_rules1;
    //QList<int> number_rules2;
}

void Grammar::elimination_of_left_recursion(){
    //запишем все правила вывода из А в виде А->Аа1...
    //QList<int> number_rules1;
    //QList<int> number_rules2;
    int number1 = 0;
    int number2;
    QStringList last_keys;
    for(auto& iter : production.keys()){
        number2 = 0;
        if(last_keys.contains(iter))
            continue;
        for(auto& iter2 : production.values(iter)){
            if(iter2.first().name == iter){
                //qDebug()<<iter<<iter2.at(1).name<<iter2.at(2).name;
                //production.find(iter,iter2)->removeFirst();
                //удаляем рекурсию, вставляем в конец новый символ с очередной цифрой и запоминаем позиции
                auto& iternew = iter+QString::number(number2);
                this->nonterminalsymbolsname.append(iternew);
                production.insertMulti(production.find(iter,iter2),iternew,iter2);
                production.remove(iter,iter2);
                production.find(iternew,iter2)->removeFirst();
                iter2.removeFirst();
                symbol new_symbol;
                new_symbol.name = iternew;
                new_symbol.type = false;
                production.find(iternew,iter2)->append(new_symbol);
                iter2.append(new_symbol);

                //добавляем переход на empty для нового символа
                symbol new_empty_symbol;
                new_empty_symbol.name = "empty";
                new_empty_symbol.type = false;
                QList<symbol> new_empty_symbol_list;
                new_empty_symbol_list.append(new_empty_symbol);
                production.insertMulti(iternew,new_empty_symbol_list);
                //production.find(iternew,iter2)->append(new_empty_symbol);
                //iter2.append(new_empty_symbol);

                //добавляем новый символ в конец всех остальных правил
                for(auto& iter3 : production.values(iter)){
                    production.find(iter,iter3)->append(new_symbol);
                }

                //number_rules2.append(number2);
            }
            number2++;
        }
        number1++;
        last_keys.append(iter);
    }
    //qDebug()<<number_rules1;
    //qDebug()<<number_rules2;
    /*for(auto& iter : production.keys()){
        //number2 = 0;
        if(last_keys.contains(iter))
            continue;
        for(auto& iter2 : production.values(iter)){
            if(number_rules1.indexOf(number1) != -1){
                symbol new_symbol;
                new_symbol.name = iter+QString::number(number_rules2.at(number_rules1.indexOf(number1)));
                new_symbol.type = true;
                production.find(iter,iter2)->append(new_symbol);
            }
            //if(number_rules1.contains(number1) && number_rules2.contains(number2) && number_rules1.indexOf(number1) == number_rules2.indexOf(number2)){
                //iter2.removeAt(0);
            //}
            //number2++;
        }
        number1++;
        last_keys.append(iter);
    }*/
}

void Grammar::debug_print(){
    qDebug()<<terminalsymbolsname;
    qDebug()<<terminalsymbolsspell;
    qDebug()<<nonterminalsymbolsname;
    qDebug()<<startSymbol;
    QStringList last_keys;
    for(auto& iter : production.keys()){
        if(last_keys.contains(iter))
            continue;
        for(auto& iter2 : production.values(iter)){
        qDebug()<<"lhs:"<<iter;
        qDebug()<<"rhs:";
            for(auto& iter3 : iter2){
                qDebug()<<iter3.name<<","<<iter3.type;
            }
        }
        last_keys.append(iter);
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
    if(isValid){
        production.clear();
        terminalsymbolsname.clear();
        terminalsymbolsspell.clear();
        nonterminalsymbolsname.clear();
        write_info();
        elimination_of_left_recursion();
        remove_useless_characters();
        make_output();
        //debug_print();
    }
}

Grammar::make_output(){
     QJsonObject parentObject;
     QJsonArray grammararray;
     QJsonObject terminalsymbols;
     QJsonObject term;
     QJsonObject nonterm;
     QJsonObject productions;
     QJsonObject production;
     QJsonObject nonterminalsymbols;
     //
     QJsonObject startsymbol;
     QJsonObject startsymbolname;
     startsymbolname.insert("-name",this->startSymbol);
     terminalsymbols.insert("terminalsymbols", QJsonValue::fromVariant(term));
     //

     //
     QJsonArray nontermelem;
     QJsonObject nontermname;
     for(QString iter : this->nonterminalsymbolsname){
         nontermname.insert("-name",QJsonValue::fromVariant(iter));
         nontermelem.append(QJsonValue::fromVariant(nontermname));
     }
     nonterm.insert("nonterm", QJsonValue::fromVariant(nontermelem));
     nonterminalsymbols.insert("nonterminalsymbols", QJsonValue::fromVariant(nonterm));
     //

     //
     QJsonArray termelem;
     QJsonObject termname;
     int number_iter = 0;
     for(QString iter : this->terminalsymbolsname){
         termname.insert("-name",QJsonValue::fromVariant(iter));
         termname.insert("-spell",QJsonValue::fromVariant(this->terminalsymbolsspell.at(number_iter)));
         termelem.append(QJsonValue::fromVariant(termname));
         number_iter++;
     }
     term.insert("term", QJsonValue::fromVariant(termelem));
     terminalsymbols.insert("terminalsymbols", QJsonValue::fromVariant(term));
     //

     //
     QJsonArray productionname;
     QJsonObject productionelem;
     //number_iter = 0;
     QStringList last_keys;
     for(QString iter : this->production.keys()){
         if(last_keys.contains(iter))
             continue;
         for(auto& symbol_iter : this->production.values(iter)){
             QJsonObject productionlhsname;
             QJsonObject productionrhssymbol;
             QJsonArray productionrhssymbolname;
             for(auto& symbol_iter2 : symbol_iter){
                 QJsonObject symbol_elem;
                 if(symbol_iter2.type)
                    symbol_elem.insert("-type",QJsonValue::fromVariant("term"));
                 else
                    symbol_elem.insert("-type",QJsonValue::fromVariant("nonterm"));
                 symbol_elem.insert("-name",QJsonValue::fromVariant(symbol_iter2.name));
                 productionrhssymbolname.append(symbol_elem);
             }
             productionrhssymbol.insert("symbol",productionrhssymbolname);
             productionlhsname.insert("-name",QJsonValue::fromVariant(iter));
             productionelem.insert("lhs",QJsonValue::fromVariant(productionlhsname));
             productionelem.insert("rhs",QJsonValue::fromVariant(productionrhssymbol));
             productionname.append(productionelem);
         }
         //number_iter++;
         last_keys.append(iter);
     }
     production.insert("production",QJsonValue::fromVariant(productionname));
     //

     productions.insert("productions", QJsonValue::fromVariant(production));
     startsymbol.insert("startsymbol", QJsonValue::fromVariant(startsymbolname));
     grammararray.append(terminalsymbols);
     grammararray.append(nonterminalsymbols);
     grammararray.append(productions);
     grammararray.append(startsymbol);
     parentObject.insert("grammar", QJsonValue::fromVariant(grammararray));
     output.setObject(parentObject);
}

