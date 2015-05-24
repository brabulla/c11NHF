#include <iostream>
#include "Expressions.h"
#include <SDL2/SDL.h>
#include <sstream>
#include <deque>
#include <vector>
#include <regex>
using namespace std;

/**
 * Converts an expression, which is in raw string format to valid Reverse Polish Notation form.
 * @param exp the string to be parsed
 * @return the expression in RPN form
 */
std::string parseString(string exp) {
    deque<string> out;
    deque<string> opStack;
    deque<int> parenthesisStack;
    stringstream ss;
    string ret;
    bool wasChar = false;
    bool wasFunc=false;
    string funcName;
    for (auto it : exp) {
        if (!(isdigit(it) || it=='.'))
            wasChar = false;
        if (isdigit(it) || it=='.') {
            if (wasChar) {
                out.pop_back();
                string temp = out.back();
                out.pop_back();
                out.push_back(temp + string(1, it));
            } else {
                out.push_back(string(1, it));
            }
            out.push_back(" ");
            wasChar = true;
        } else if (it == '+' || it == '-') {
            for (auto itOut: opStack) {
                if (!itOut.compare("+") || !itOut.compare("-") || !itOut.compare("*") || !itOut.compare("/") ||
                    !itOut.compare("^")) {
                    out.push_back(opStack.front());
                    out.push_back(" ");
                    opStack.pop_front();
                } else
                    break;
            }
            opStack.push_front(string(1, it));
        } else if (it == '*' || it == '/') {
            for (auto itOut: opStack)
                if (!itOut.compare("*") || !itOut.compare("/") || !itOut.compare("^")) {
                    out.push_back(opStack.front());
                    out.push_back(" ");
                    opStack.pop_front();
                } else
                    break;
            opStack.push_front(string(1, it));
        } else if (it == '^') {
            opStack.push_front(string(1, it));
        } else if (it == '(') {
            if(wasFunc){
                opStack.push_front(funcName);
                wasFunc=false;
                funcName.clear();
                parenthesisStack.push_front(0);
            }
            opStack.push_front(string(1, it));
            if(parenthesisStack.size()){
                parenthesisStack.front()++;
            }
        } else if (it == ')') {
            for (auto itOut: opStack)
                if (!itOut.compare("(")) {
                    opStack.pop_front();
                    break;
                }
                else {
                    out.push_back(opStack.front());
                    out.push_back(" ");
                    opStack.pop_front();
                }
            if(parenthesisStack.size()){
                parenthesisStack.front()--;
                if(parenthesisStack.front()==0){
                    parenthesisStack.pop_front();
                    out.push_back(opStack.front());
                    out.push_back(" ");
                    opStack.pop_front();
                }
            }
        } else if (it == 'X') {
            out.push_back(string(1, it));
            out.push_back(" ");
        } else if (it == ' ') {

        } else {
            wasFunc=true;
            funcName.append(string(1,it));
        }
    }
    for (auto it : opStack) {
        out.push_back(it);
        out.push_back(" ");
    }
    for (auto it : out)
        ss << it;
    ret = ss.str();
    return ret;
}

/**
 * String splitter function, splits a string at delim characters
 * @param s string for splitting
 * @param delim delimiter character
 * @return split vector of strings
 */
std::vector<std::string> split(const std::string &s, char delim) {
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> splitted;
    while (std::getline(ss, item, delim))
        splitted.push_back(item);
    return splitted;
}

/**
 * Ceates a new function object depending on the incoming string.
 * Implement new function types here.
 * @param s function name to be parsed
 * @return function object, containing the parsed function
 */
std::function<double(double)> parseFunction(const std::string &s) {
    if (!s.compare("sin")){
        return [](double x){return sin(x);};
    }else if(!s.compare("cos")){
        return [](double x){return cos(x);};
    }else if(!s.compare("tan")){
        return [](double x){return tan(x);};
    }else if(!s.compare("abs")){
        return [](double x){return abs(x);};
    }else{
        throw -2;
    }

}
/**
 * Gets an expression in Reverse Polish Notation, and builds an Expression Tree from it.
 * Normal RPN evaluation, only it doesn't do any primitive calculation, just evaluates the next token, generates the
 * required Expression, and pushes it into a stack.
 * @param RPNExp string, in Reverse Polish Notation format.
 * @return pointer to the Expression Tree
 */
Expression *buildTree(std::string RPNExp) {
    std::vector<std::string> RPNTokens = split(RPNExp, ' ');
    std::deque<Expression *> expStack;
    for (auto item : RPNTokens) {
        try {
            if (std::regex_match(item, std::regex("[[:digit:]]+.?[[:digit:]]*"))) {
                expStack.push_back(new Constant(atof(item.c_str())));
            } else if (std::regex_match(item, std::regex("[+-/*^]"))) {
                std::unique_ptr<Expression> rhs = expStack.back();
                expStack.pop_back();
                std::unique_ptr<Expression> lhs = expStack.back();
                expStack.pop_back();
                switch (item.at(0)) {
                    case '+':
                        expStack.push_back(new Sum{lhs, rhs});
                        break;
                    case '*':
                        expStack.push_back(new Prod{lhs, rhs});
                        break;
                    case '/':
                        expStack.push_back(new Div{lhs, rhs});
                        break;
                    case '-':
                        expStack.push_back(new Dif{lhs, rhs});
                        break;
                    case '^':
                        expStack.push_back(new Exp{lhs, rhs});
                        break;
                }
            } else if (std::regex_match(item, std::regex("X"))) {
                expStack.push_back(new Variable{});
            } else{
                try{
                    std::function<double(double)> func{parseFunction(item)};
                    std::unique_ptr<Expression> arg = expStack.back();
                    expStack.pop_back();
                    expStack.push_back(new Function{arg.rele,func,item});
                }catch (...){

                }
            }
        } catch (std::regex_error e) {
            cout << e.code();
        }
    }
    return expStack.front();
}

/**
 * Function to draw the Expression.
 * Clears the screen, draws the axes, then draws the function.
 * @param window pointer to the SDL window
 * @param exp pointer to the Expression Tree
 * @param maxX max X value to draw
 * @param maxY max Y value to draw
 */
void drawFunction(SDL_Window * window, unique_ptr<Expression> exp,int maxX,int maxY){

    SDL_Surface* screenSurface = SDL_GetWindowSurface(window);
    SDL_Renderer* renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);

    int screenw,screenh;
    screenw=screenSurface->w;
    screenh=screenSurface->h;

    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer,255,255,255,0);
    SDL_Rect rectangle;
    rectangle.x=rectangle.y=0;
    rectangle.w=rectangle.h=600;

    SDL_RenderFillRect(renderer,&rectangle);
    SDL_SetRenderDrawColor(renderer,0,0,0,0);
    SDL_RenderDrawLine(renderer,screenw/2,0,screenw/2,screenh);
    SDL_RenderDrawLine(renderer,0,screenh/2,screenw,screenh/2);
    SDL_SetRenderDrawColor(renderer,255,0,0,0);

    double ratiox=screenw/(maxX*2);
    double ratioy=screenh/(maxY*2);

    for(int i=1;i<screenw;i++){
        int x1,x2,y1,y2;
        x1=i-1;
        x2=i;
        y1=screenh-(exp->evaluate(x1/ratiox-maxX)+maxY)*ratioy;
        y2=screenh-(exp->evaluate(x2/ratiox-maxX)+maxY)*ratioy;
        SDL_RenderDrawLine(renderer,x1,y1,x2,y2);
    }
    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[]) {

    //Functions you could try with:
    //X-3
    //X + 4 ^ 2 * 2 / (5 - 1)
    //abs((sin(X))
    int maxX,maxY;
    string func;
    cout<< "Maximum az X tengelyen?: ";
    cin>>maxX;
    cout<< "Maximum az Y tengelyen?: ";
    cin>>maxY;
    cout<<"Kirajzolando fuggveny?: ";
    cin>>func;

    //string s = parseString("X + 4 ^ 2 * 2 / (5 - 1) ");
    //string s2 = parseString("abs(sin(X))");

    std::unique_ptr<Expression> e = buildTree(parseString(func));
    std::unique_ptr<Expression> esimpl=e->simplify();
    //The window we'll be rendering to
    SDL_Window *window = NULL;

    //The surface contained by the window
    SDL_Surface *screenSurface = NULL;

    SDL_Renderer *renderer=NULL;
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cout << "SDL could not initialize!";
    }
    else {
        //Create window
        window = SDL_CreateWindow("Function drawer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 600,
                                  SDL_WINDOW_SHOWN);
        if (window == NULL) {
            cout << "No window" << endl;
        }
        else {

            drawFunction(window,esimpl,maxX,maxY);
            while (1) {
                SDL_Event e;
                if (SDL_PollEvent(&e)) {
                    if (e.type == SDL_QUIT) {
                        break;
                    }
                }
            }

            //Destroy window
            SDL_DestroyWindow(window);

            //Quit SDL subsystems
            SDL_Quit();
        }
    }



    return 0;
};

