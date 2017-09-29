#pragma once
#include<chrono>
#include<sstream>
#include<iostream>
#include<cmath>
#include<array>
struct Timer{
    using time_t=std::chrono::time_point<std::chrono::system_clock>;
    time_t start_;
    Timer():
        start_(std::chrono::system_clock::now())
    {}

    void reset()
    {
        start_=std::chrono::system_clock::now();
    }

    double get_time()const{
        time_t end=std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end-start_;
        return elapsed_seconds.count();
    }
};

void print_banner(const std::string& msg)
{
    std::cout<<std::string(80,'=')<<std::endl;
    const size_t len=msg.size();
    if(len<76){
        size_t extra=(76-len)%2;
        size_t base=(76-len-extra)/2;
        std::string lbuffer(base+extra,' '),rbuffer(base,' ');
        std::cout<<"= "<<lbuffer<<msg<<rbuffer<<" ="<<std::endl;
    }
    else//Not going to fit in 80 columns....
        std::cout<<msg<<std::endl;
    std::cout<<std::string(80,'=')<<std::endl;
}

template<size_t N>
std::string elem_name(const std::array<size_t,N>& idx)
{
    std::string rv("Element {");
    for(auto x:idx)rv+=std::to_string(x)+",";
    rv+="}";
    return rv;
}

struct Tester
{
    size_t nfailed_=0;
    size_t ntests_=0;
    Tester(const std::string& msg)
    {
        print_banner(msg);
    }

    void test(const std::string& msg, bool passed)
    {
        ++ntests_;
        std::cout<<"Test "<<std::to_string(ntests_)<<" : "<<msg<<" passed? : "
                 <<std::boolalpha<<passed<<std::endl;
        if(!passed)++nfailed_;

    }
    size_t results()
    {
        std::stringstream ss;
        ss<<std::to_string(nfailed_)<<" tests failed out of "
          <<std::to_string(ntests_)<<"!!!!! ";
        if(nfailed_)ss<<"Sad face.";
        else ss<<"Happy face.";
        print_banner(ss.str());
        return nfailed_;
    }
};

template<typename T1, typename T2>
bool are_same(const T1& lhs, const T2& rhs,double tol=1E-6)
{
    for(size_t i=0;i<lhs.size();++i)
        if(std::fabs(lhs[i]-rhs[i])>tol)return false;
    return true;
}
