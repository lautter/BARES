#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#ifndef DEFINITIVO
#include <stack>
#include <queue>
#define Pilha std::stack
#define Fila std::queue
#endif
bool isNotMinus(char c){
    return c=='-'? false:true;
}
template <typename T>
std::string to_string(T value){
    /**
    * Converts a string to an integer
    */
    //create an output string stream
    std::ostringstream os ;
    //throw the value into the string stream
    os << value ;
      //convert the string stream into a string and return
    return os.str() ;
}

bool isValOpd(std::string str){         //Provável bug na conversao de string pra inteiro
    /**
    * is valid operand
    */
    int v=atoi(str.c_str());
    if(v>32767||v<(-32767)) return false;
    return true;
}
std::string errorString(int i,int col){
    /**
    * Retorna a string correspondente ao erro do tipo i na coluna col
    */
    std::string str[]={"Constante numérica inválida: um dos operandos da expressão está fora da faixa permitida. (Coluna: ",
                        "Falta operando: em alguma parte da expressão está faltando um operando . (Coluna: ",
                        "Operando inválido: existe um operando que não é constante numérica. (Coluna: ",
                        "Operador inválido: existe um símbolo correspondente a um operador que não está na lista de operadores válidos. (Coluna: ",
                        "Falta operador: em alguma parte da expressão está faltando um operador. (Coluna: ",
                        "Fechamento de escopo inválido: existe um parêntese fechando sem haver um parêntese abrindo correspondente.",
                        "Escopo aberto: existe um parêntese de abertura '(' sem um parêntese de fechamento ')' correspondente.",
                        "Divisão por zero: Houve uma divisão cujo quociente é zero. Coluna: ",
                        "Parênteses desnecessários: Parênteses sem nada em seu interior. Coluna: "};
    return str[i-1]+to_string(col)+")";
}

int extSintFromStr(std::string &num,std::string str){
    /**
    * Extrai um inteiro de uma string e retorna o indice no qual aparece um caractere na string que nao e mais digito
    */
    unsigned int i;
    for(i=0;i<str.length()&&isdigit(str[i])&&str[i]!='\n';i++);

    num+=str.substr(0,i);
    return --i;
}

bool isParnt(char c){
    if(c=='('||c==')')
        return true;
    return false;
}

bool isValOpt(char c){
    if(c=='%'||c=='*'||c=='+'||c=='-'||c=='/'||c=='^')
        return true;
    return false;
}
bool isnValOpt(char c){                             //Se later
    if(c==','||c=='.'||c=='<'||c=='>'||c=='=')
        return true;
    return false;
}
bool isIgnChar(char c){
    if(c==32||c==9)
        return true;
    return false;
}
bool isValChar(char c){
    if(isdigit(c)||isValOpt(c)||isIgnChar(c)||isParnt(c))
        return true;
    return false;
}
int extSMinusFromStr(std::string &num,std::string str){
    /**
    * Analisa uma sequencia de operadores '-'
    */
    unsigned int i;
    unsigned int contIgnC=0;
    for(i=0;i<str.length()&&
            (!isNotMinus(str[i])||isIgnChar(str[i]))
            &&str[i]!='\n';i++)
                if(isIgnChar(str[i])) contIgnC++;
    if((str.substr(0,i).length()-contIgnC)%2==0) num="";
    else{ num="-";}
    return --i;
}
template <typename T>
bool tokeniza(Expressao entrada,Fila<Simbolo> &tokens,Fila<Erro> &erros){
    Pilha<unsigned int> contPar;                            //Pilha controladora de parenteses
    Simbolo token;                                        //Buffer do token
    bool isLastOpt=true;                               //is the last a operator?
    bool isLastUnrMinus=false;                               //is the last a operator?
    for(unsigned int i=0;i<entrada.getExp().length();i++){
        if(entrada.isValChar(i)){
            if(entrada.isDigit(i)){
                if(!isLastOpt&&token!="("){
                    erros.push(Erro(5,i-1));
                }
                if(!isLastUnrMinus)
                    token="";

                int aux=extSintFromStr(token,entrada.getExp().substr(i,entrada.getExp().length()));
                if(!isValOpd(token)){
                    erros.push(Erro(1,i));
                    token="0";
                }
                token.setCol(i);
                tokens.push(token);
                i+=aux;
                isLastOpt=0;
                isLastUnrMinus=0;
            }else if(entrada.isParnt(i)){////////
                if(entrada.getExp()[i]=='('){
                    if(!isLastOpt&&token!="(")
                        erros.push(Erro(5,i));
                    contPar.push(i);
                }else{
                    if(isLastOpt&&!tokens.empty())
                        erros.push(Erro(2,i));       //ERROR: ultimo eh operador
                    if(!contPar.empty()){                   //Se houver parentese abraindo
                        if(token=="(")
                            erros.push(Erro(9,i));
                        contPar.pop();
                    }else
                        erros.push(Erro(6,i));       //ERROR: fechando sem abrir
                }

                if(!isLastUnrMinus)
                    token="";
                token+=entrada.getExp()[i];
                tokens.push(token);

                isLastOpt=0;
                isLastUnrMinus=0;
            }else if(isValOpt(entrada.getExp()[i])){
                if(isNotMinus(entrada.getExp()[i])){
                    if(!isLastOpt){
                        if(!contPar.empty()){
                            if(token=="(")
                                erros.push(Erro(2,i));
                        }
                    }else
                        erros.push(Erro(2,i));

                    if(!isLastUnrMinus)
                        token="";
                    token+=entrada.getExp()[i];
                    isLastUnrMinus=0;
                }else{
                    std::string num;
                    int aux=extSMinusFromStr(num,entrada.getExp().substr(i,entrada.getExp().length()));
                    if(token.length()>0){
                        if(isLastOpt||token=="("){
                            isLastUnrMinus=1;
                        }else isLastUnrMinus=0;
                    }token=num;
                    i+=aux;
                }
                if(!isLastUnrMinus&&token.length()>0){
                    tokens.push(token);
                }
                isLastOpt=1;
            }
        }else if(isnValOpt(entrada.getExp()[i])){
            erros.push(Erro(4,i+1));
            isLastOpt=0;
            if(!isLastUnrMinus)
                token="";
            token+=entrada.getExp()[i];
            tokens.push(token);
            isLastUnrMinus=0;
        }else if(entrada.getExp()[i]=='\n'){
            if(isLastOpt){
                erros.push(Erro(2,i));
            }
            if(!contPar.empty()) erros.push(Erro(7,i));
            break;
        }else{
            erros.push(Erro(3,i+1));
            isLastOpt=0;
            if(!isLastUnrMinus)
                token="";
            token+=entrada.getExp()[i];
            tokens.push(token);
            isLastUnrMinus=0;
        }if(!tokens.empty()) std::cout<<token<<std::endl;
    }

    if(erros.empty())
        return true;
    while(!tokens.empty()){
        tokens.pop();
    }
    return false;
}
template<typename T>
void imprime(Fila<T> fila){
    std::cout<<"[";
    while(!fila.empty()){
        std::cout<<fila.front()<<",";
        fila.pop();
    }std::cout<<"[";

}
int main(void){
    std::string a="1^2*3+(-4)+5/6/(7+8)";
    Fila<std::string> fila;
    Fila<std::string> erros;
    if(tokeniza(a,fila,erros)){
        imprime(fila);
    }else imprime(erros);
    return 0;
}
