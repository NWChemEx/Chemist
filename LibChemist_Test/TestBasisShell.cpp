#include "LibChemist/BasisShell.hpp"
#include "TestHelpers.hpp"

using namespace LibChemist;


int main()
{
    Tester tester("Testing BasisShell class");

    //Make sure we can construct a default BasisShell
    BasisShell Defaulted;

    //Some fake exponents
    std::vector<double> as({3.1,4.5,6.9});
    //Some fake contraction coefficients
    std::vector<double> cs({8.1,2.6,7.1});
    //Some fake contraction coefficients assuming a general contraction
    std::vector<double> gen_cs({1.4,6.8, 7.1,9.1, 5.4, 6.0});

    BasisShell CartBS(ShellType::CartesianGaussian,2,1,as,cs);
    BasisShell PureBS(ShellType::SphericalGaussian,-1,2,std::move(as),
                      std::move(gen_cs));
    tester.test("Cart shell type",CartBS.type==ShellType::CartesianGaussian);
    tester.test("Pure shell type",PureBS.type==ShellType::SphericalGaussian);
    tester.test("Angular momentum",CartBS.l==2);
    tester.test("# contractions",PureBS.ngen==2);
    tester.test("NPrims",CartBS.nprim==3);
    tester.test("Cart NFunctions",CartBS.nfunctions(0)==6);
    tester.test("Pure NFunctions contraction 1",PureBS.nfunctions(0)==1);
    tester.test("Pure NFunctions contraction 2",PureBS.nfunctions(1)==3);

    BasisShell Copy(CartBS);
    tester.test("Copy constructor",Copy==CartBS);
    tester.test("Not equal",Copy!=PureBS);
    BasisShell Moved(std::move(Copy));
    tester.test("Move constructor",Moved==CartBS);
    Defaulted=std::move(Moved);
    tester.test("Move assignment",Defaulted==CartBS);
    Moved=Defaulted;
    tester.test("Assignment",Moved==Defaulted && Moved==CartBS);
    tester.test("Get exponent",CartBS.alpha(1)==4.5);
    tester.test("Get coefficient",CartBS.coef(1,0)==2.6);
    tester.test("General get coef",PureBS.coef(1,1)==5.4);


    return tester.results();
}
