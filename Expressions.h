//
// Created by Adam on 2015.05.14..
//

#ifndef C11NHF_EXPRESSIONS_H
#define C11NHF_EXPRESSIONS_H
#include <iostream>
#include <memory>
/**
 * Abstract expression base class, Expression implementations inherit from this.
 */
class Expression {
public:
    /**
     * Returns with the value of the Expression at place X.
     * @param x place to evaluate expression at.
     * @return value of the expression
     */
    virtual double evaluate(double x) const = 0;

    /**
     * Prints the signature of the expression to os;
     * @param os ostream object, where the function prints the signature
     */
    virtual void print(std::ostream &os) const = 0;

    /**
     * Virtual constructor for the class.
     * @return new object of the same type.
     */
    virtual Expression * clone() const = 0 ;

    /**
     * If the expression can be simplified, returns a new, simplified version
     * @return simplified expression
     */
    virtual std::unique_ptr<Expression> simplify() const;

};

/**
 * Expression that represents a constant.
 */
class Constant final  : public Expression {
public:
    double c;
    Constant(double inC);

    /**
     * @see Expression::evaluate()
     */
    virtual double evaluate(double x) const override;

    /**
     * @see Expression::print()
     */
    virtual void print(std::ostream &os) const override;

    /**
     * @see Expression::clone()
     */
    virtual Constant* clone() const override;

    /**
     * Returns the value of the constant.
     * @return value of constant.
     */
    double get_value() const;
};

/**
 * Expression representing a variable.
 */
class Variable final : public Expression {
public:

    /**
     * @see Expression::evaluate()
     */
    virtual double evaluate(double x) const override;

    /**
     * @see Expression::print()
     */
    virtual void print(std::ostream &os) const override;

    /**
     * @see Expression::clone()
     */
    virtual Variable* clone() const override;
};

/**
 * Class representing an operation with two arguments. Base class for further implementation
 */
class TwoOperand : public Expression{

public:
    std::unique_ptr<Expression> lhs,rhs;
    TwoOperand(std::unique_ptr<Expression>&& inLhs, std::unique_ptr<Expression>&& inRhs);

    /**
     * Copy contructor, creating a deep copy of the other objects attributes.
     * @param other other TwoOperand obect, to use as base
     */
    TwoOperand(TwoOperand const & other);

    /**
     * Executes the operator represented by the class on two values.
     * @param lhs left hand side argument of the operation
     * @param rhs right hand side argument of the operator
     * @return result of the operation
     */
    virtual double do_operator(double lhs, double rhs) const =0;

    /**
     * @see Expression::evaluate()
     */
    virtual double evaluate(double x) const override;

    /**
     * Return the signature character representing the operation.
     * @return char representing the operation
     */
    virtual char get_operator() const =0;

    /**
     * @see Expression::print()
     */
    virtual void print(std::ostream &os) const override;

    /**
     * @see Expression::clone()
     */
    virtual TwoOperand* clone() const =0;

    /**
     * @see Expression::simplify()
     */
    virtual std::unique_ptr<Expression> simplify() const = 0;
};

/**
 * Class implementing the Sum operation of two arguments.
 */
class Sum final: public TwoOperand{
public:
    Sum(std::unique_ptr<Expression>&& inLhs, std::unique_ptr<Expression>&& inRhs)  : TwoOperand(std::move(inLhs), std::move(inRhs)) {}

    /**
     * Adds the two arguments together and returns the result.
     * @see TwoOperand::do_operator()
     */
    virtual double do_operator(double lhs, double rhs) const override;

    /**
     * @see TwoOperand::get_operator()
     */
    virtual char get_operator() const override;

    /**
     * @see Expression::clone()
     */
    virtual Sum *clone() const override;

    /**
     * @see Expression::simplify()
     */
    virtual std::unique_ptr<Expression> simplify() const override;

};

/**
 * Class implementing the multiplication operation of two arguments.
 */
class Prod final: public TwoOperand{
public:
    Prod(std::unique_ptr<Expression>&& inLhs,std::unique_ptr<Expression>&& inRhs) : TwoOperand(std::move(inLhs), std::move(inRhs)) { }

    /**
     * Multiplies the two arguments together and returns the result.
     * @see TwoOperand::do_operator()
     */
    virtual double do_operator(double lhs, double rhs) const override;

    /**
     * @see TwoOperand::get_operator()
     */
    virtual char get_operator() const override;

    /**
     * @see Expression::clone()
     */
    virtual Prod *clone() const override;

    /**
     * @see Expression::simplify()
     */
    virtual std::unique_ptr<Expression> simplify() const override;

};

/**
 * Class implementing the difference operation of two arguments.
 */
class Dif final: public TwoOperand{
public:

    Dif(std::unique_ptr<Expression>&& inLhs, std::unique_ptr<Expression>&& inRhs) : TwoOperand(std::move(inLhs), std::move(inRhs)) { }

    /**
     * Substracts the rhs from the lhs and returns the result.
     * @see TwoOperand::do_operator()
     */
    virtual double do_operator(double lhs, double rhs) const override;

    /**
     * @see TwoOperands::get_operator()
     */
    virtual char get_operator() const override;

    /**
     * @see Expression::clone()
     */
    virtual Dif *clone() const override;

    /**
     * @see Expression::simplify()
     */
    virtual std::unique_ptr<Expression> simplify() const override;
};

/**
 * Class implementing the division operation of two arguments.
 */
class Div final: public TwoOperand{
public:

    Div(std::unique_ptr<Expression>&& inLhs,std::unique_ptr<Expression>&& inRhs) : TwoOperand(std::move(inLhs), std::move(inRhs)) { }

    /**
     * Divides the lhs with the rhs and returns the result.
     * @see TwoOperand::do_operator()
     */
    virtual double do_operator(double lhs, double rhs) const override;

    /**
    * @see TwoOperands::get_operator()
    */
    virtual char get_operator() const override;

    /**
    * @see Expression::clone()
    */
    virtual Div* clone() const override;
    /**
     * @see Expression::simplify()
     */
    virtual std::unique_ptr<Expression> simplify() const override;
};

/**
 * Class implementing the power operation of two arguments.
 */
class Exp final: public TwoOperand{
public:
    Exp(std::unique_ptr<Expression>&& inLhs,std::unique_ptr<Expression>&& inRhs) : TwoOperand(std::move(inLhs), std::move(inRhs)) { }

    /**
     * Places lhs to the rhs-th power.
     * @see TwoOperand::do_operator()
     */
    virtual double do_operator(double lhs, double rhs) const override;

    /**
    * @see TwoOperands::get_operator()
    */
    virtual char get_operator() const override;

    /**
    * @see Expression::clone()
    */
    virtual Exp *clone() const override;

    virtual std::unique_ptr<Expression> simplify() const override;
};

/**
 * Class implementing a function with double(double) signature.
 */
class Function : public Expression{
public:
    std::function<double(double)> functor;

    std::unique_ptr<Expression> arg;

    std::string name;

    Function(std::unique_ptr<Expression>&& inArg,std::function<double(double)> func, std::string name );

    Function(const Function& in);

    /**
     * @see Expression::evaluate()
     */
    virtual double evaluate(double x) const override;

    /**
     * @see Expression::print()
     */
    virtual void print(std::ostream &os) const override;

    /**
     * @see Expression::clone()
     */
    virtual Expression *clone() const override;

    /**
     * @see Expression::simplify()
     */
    virtual std::unique_ptr<Expression> simplify() const override;
};
#endif //C11NHF_EXPRESSIONS_H
