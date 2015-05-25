#include <stdexcept>
#include <math.h>
#include "Expressions.h"

std::unique_ptr<Expression> Expression::simplify() const {
    return std::unique_ptr<Expression>(clone());
}
Constant::Constant(double inC) : c{inC} { }

double Constant::evaluate(double x) const {
    return c;
}

void Constant::print(std::ostream &os) const {
    os << c;
}

Constant* Constant::clone() const{
    return new Constant{*this};
}

double Variable::evaluate(double x) const {
    return x;
}

void Variable::print(std::ostream &os) const {
    os << 'x';
}

Variable* Variable::clone() const{
    return new Variable();
}

std::ostream &operator<<(std::ostream &os, Expression const &e) {
    e.print(os);
    return os;
}

TwoOperand::TwoOperand(std::unique_ptr<Expression>&& inLhs, std::unique_ptr<Expression>&& inRhs) : lhs{std::move(inLhs)}, rhs{std::move(inRhs)} {}

TwoOperand::TwoOperand(TwoOperand const& other): lhs{other.lhs->clone()}, rhs{other.rhs->clone()} {}

double TwoOperand::evaluate(double x) const {
    return do_operator(lhs->evaluate(x), rhs->evaluate(x));
}

void TwoOperand::print(std::ostream &os) const {
    os << '(' << *lhs << get_operator() << *rhs << ')';
}

double Sum::do_operator(double lhs, double rhs) const {
    return lhs + rhs;
}

char Sum::get_operator() const {
    return '+';
}

Sum * Sum::clone() const {
    return new Sum{*this};;
};

std::unique_ptr<Expression> Sum::simplify() const {
    std::unique_ptr<Expression> lhs_simpl {lhs->simplify()};
    std::unique_ptr<Expression> rhs_simpl {rhs->simplify()};
    std::unique_ptr<Constant> lhs_cons{dynamic_cast<Constant *>(lhs_simpl->clone())};
    std::unique_ptr<Constant> rhs_cons{dynamic_cast<Constant *>(rhs_simpl->clone())};
    if (lhs_cons && lhs_cons->get_value() == 0.0) {  /* 0 + a = a */
        return std::unique_ptr<Expression>(rhs_simpl.release());
    }
    if (rhs_cons != nullptr && rhs_cons->get_value() == 0.0) {  /* a + 0 = a */
        return std::unique_ptr<Expression>(lhs_simpl.release());
    }
    if (lhs_cons != nullptr && rhs_cons != nullptr) {   /* c + c = c */
        double new_value = lhs_cons->get_value() + rhs_cons->get_value();
        return std::unique_ptr<Expression>(new Constant{new_value});
    }
    return std::unique_ptr<Expression>(new Sum{std::move(lhs_simpl), std::move(rhs_simpl)});
};

double Prod::do_operator(double lhs, double rhs) const {
    return lhs * rhs;
}

char Prod::get_operator() const {
    return '*';
}

std::unique_ptr<Expression> Prod::simplify() const {
    std::unique_ptr<Expression> lhs_simpl {lhs->simplify()};
    std::unique_ptr<Expression> rhs_simpl {rhs->simplify()};
    std::unique_ptr<Constant> lhs_cons{dynamic_cast<Constant *>(lhs_simpl->clone())};
    std::unique_ptr<Constant> rhs_cons{dynamic_cast<Constant *>(rhs_simpl->clone())};
    if (lhs_cons && lhs_cons->get_value() == 1.0) {  /* 1 * a = a */
        return std::unique_ptr<Expression>(rhs_simpl.release());
    }
    if ((rhs_cons && rhs_cons->get_value()==0.0) || (lhs_cons && lhs_cons->get_value() == 0.0)) {  /* 0 * a = 0 || a * 0 = 0 */
        return std::unique_ptr<Expression>(new Constant{0});
    }
    if (rhs_cons && rhs_cons->get_value() == 1.0) {  /* a + 1 = a */
        return std::unique_ptr<Expression>(lhs_simpl.release());
    }
    if (lhs_cons && rhs_cons ) {   /* c * c = C */
        double new_value = lhs_cons->get_value() * rhs_cons->get_value();
        return std::unique_ptr<Expression>(new Constant{new_value});
    }
    return std::unique_ptr<Expression>(new Prod{std::move(lhs_simpl), std::move(rhs_simpl)});
}

double Dif::do_operator(double lhs, double rhs) const {
    return lhs - rhs;
}

char Dif::get_operator() const {
    return '-';
}

std::unique_ptr<Expression> Dif::simplify() const {
    std::unique_ptr<Expression> lhs_simpl {lhs->simplify()};
    std::unique_ptr<Expression> rhs_simpl {rhs->simplify()};
    std::unique_ptr<Constant> lhs_cons{dynamic_cast<Constant *>(lhs_simpl->clone())};
    std::unique_ptr<Constant> rhs_cons{dynamic_cast<Constant *>(rhs_simpl->clone())};
    if (lhs_cons && lhs_cons->get_value() == 0.0) {  /* 0 - a = -1 * a */
        return std::unique_ptr<Expression>(new Prod{std::move(rhs_simpl),std::unique_ptr<Expression>(new Constant{-1.0})});
    }
    if (rhs_cons  && rhs_cons->get_value() == 0.0) {  /* a - 0  = a */
        return std::unique_ptr<Expression>(lhs_simpl.release());
    }
    if (lhs_cons && rhs_cons ) {   /* c - c = C */
        double new_value = lhs_cons->get_value() - rhs_cons->get_value();
        return std::unique_ptr<Expression>(new Constant{new_value});
    }
    return std::unique_ptr<Expression>(new Dif{std::move(lhs_simpl), std::move(rhs_simpl)});
}

double Div::do_operator(double lhs, double rhs) const {
    return lhs / rhs;
}

char Div::get_operator() const {
    return '/';
}

std::unique_ptr<Expression> Div::simplify() const {
    std::unique_ptr<Expression> lhs_simpl {lhs->simplify()};
    std::unique_ptr<Expression> rhs_simpl {rhs->simplify()};
    std::unique_ptr<Constant> lhs_cons{dynamic_cast<Constant *>(lhs_simpl->clone())};
    std::unique_ptr<Constant> rhs_cons{dynamic_cast<Constant *>(rhs_simpl->clone())};
    if (lhs_cons && lhs_cons->get_value() == 0.0) {  /* 0 / a = 0 */
        return std::unique_ptr<Expression>(new Constant{0.0});
    }
    if (rhs_cons && rhs_cons->get_value() == 0.0) {  /* a / 0  = ERR */
        throw std::runtime_error("Division by 0!");
    }
    if (lhs_cons && rhs_cons ) {   /* c / c = C */
        double new_value = lhs_cons->get_value() / rhs_cons->get_value();
        return std::unique_ptr<Expression>(new Constant{new_value});
    }
    return std::unique_ptr<Expression>(new Div{std::move(lhs_simpl), std::move(rhs_simpl)});
}

Prod *Prod::clone() const {
    return new Prod{*this};
}

Dif *Dif::clone() const {
    return new Dif{*this};
}

Div *Div::clone() const {
    return new Div{*this};
}

double Constant::get_value() const {
    return c;
}

double Exp::do_operator(double lhs, double rhs) const {
    return pow(lhs,rhs);
}

char Exp::get_operator() const {
    return '^';
}

Exp *Exp::clone() const {
    return new Exp{*this};
}

std::unique_ptr<Expression> Exp::simplify() const {
    std::unique_ptr<Expression> lhs_simpl {lhs->simplify()};
    std::unique_ptr<Expression> rhs_simpl {rhs->simplify()};
    std::unique_ptr<Constant> lhs_cons{dynamic_cast<Constant *>(lhs_simpl->clone())};
    std::unique_ptr<Constant> rhs_cons{dynamic_cast<Constant *>(rhs_simpl->clone())};
    if (lhs_cons && lhs_cons->get_value() == 1.0) {  /* 1 ^ a = 1 */
        return std::unique_ptr<Expression>(lhs_simpl.release());
    }
    if (rhs_cons && rhs_cons->get_value() == 1.0) {  /* a ^ 1 = a */
        return std::unique_ptr<Expression>(lhs_simpl.release());
    }
    if (rhs_cons && rhs_cons->get_value() == 0.0) {  /* a ^ 0 = 1 */
        return std::unique_ptr<Expression>(new Constant{1});
    }
    if (lhs_cons && rhs_cons) {   /* c * c = C */
        double new_value = pow(lhs_cons->get_value(),rhs_cons->get_value());
        return std::unique_ptr<Expression>(new Constant{new_value});
    }
    return std::unique_ptr<Expression>(new Exp{std::move(lhs_simpl), std::move(rhs_simpl)});
}

Function::Function(std::unique_ptr<Expression>&& inArg, std::function<double(double)> func, std::string name) : arg{std::move(inArg)},functor{func}, name{name}{}

Function::Function(const Function &in) : arg{in.arg->clone()},functor{in.functor}, name{in.name} { }

double Function::evaluate(double x) const {
    return functor(arg->evaluate(x));
}

void Function::print(std::ostream &os) const {
    os<<name <<'(' <<*arg <<')';
}

Expression *Function::clone() const {
    return new Function{*this};
}

std::unique_ptr<Expression> Function::simplify() const {
    return std::unique_ptr<Expression>(new Function(std::move(arg->simplify()),functor,name));
}


