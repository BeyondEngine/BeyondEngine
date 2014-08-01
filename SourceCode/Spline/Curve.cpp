#include "stdafx.h"
#include "Curve.h"
#include "Spline.h"


std::string SSpline::spline_name[SSpline::SPLINE_COUNT] = {"Cubic Spline"};

/*   aaCurve::aaCurve(const std::string &name, int knum, double *t, double *knot)
: m_name(name), m_knum(knum), m_t(0), m_knot(0){

if(m_knum <= 0)
return ;

m_t = new double[m_knum];
m_knot = new double[m_knum];

for(int i=0; i<m_knum; ++i){
m_t[i]  = t[i];
m_knot[i] = knot[i];
}

}
*/
Curve::Curve(const SSpline &spline_data)
    : m_strName(spline_data.name)
    , m_nKnum(spline_data.size())
    , m_pT(new double[m_nKnum])
    , m_pKnot(new double[m_nKnum])
{
        if(m_nKnum <= 0)
            return ;

        for(int i=0; i<m_nKnum; ++i){
            m_pT[i]  = spline_data.knots[i].t;
            m_pKnot[i] = spline_data.knots[i].y;
        }

}

Curve::~Curve(void){
    if(m_pT)
        delete []m_pT;
    if(m_pKnot)
        delete []m_pKnot;
}

bool Curve::getValue(double t, double &value){
    if(!m_pT)
        return false;

    return true;
}


//////////////////////////////////////////////////////////////////////////

CubicSpline::CubicSpline(const SSpline &spline_data)
    : Curve(spline_data)
    , m_pDdp(0)
{

        if(m_nKnum > 3)
            m_pDdp = spline_cubic_set(m_nKnum, m_pT, m_pKnot, 2, 0, 2, 0);
}

CubicSpline::~CubicSpline(void){
    if(m_pDdp)
        delete []m_pDdp;
}

bool CubicSpline::getValue(double t, double &value){
    if(!m_pT)
        return false;

    if(m_nKnum > 3){
        if(!m_pDdp)
            return false;

        double dp, ddp;
        value = spline_cubic_val(m_nKnum, m_pT, t, m_pKnot, m_pDdp, &dp, &ddp);

        return true;
    }else if(m_nKnum > 2){
        double dp;
        spline_quadratic_val(m_nKnum, m_pT, m_pKnot, t, &value, &dp);

        return true;
    }else if(m_nKnum > 1){
        double dp;
        spline_linear_val(m_nKnum, m_pT, m_pKnot, t, &value, &dp);

        return true;
    }else{
        return false;
    }
}


//////////////////////////////////////////////////////////////////////////
QuadraticSpline::QuadraticSpline(const SSpline &spline_data)
    : Curve(spline_data)
{

}

QuadraticSpline::~QuadraticSpline(void){

}

bool QuadraticSpline::getValue(double t, double &value){
    if(!m_pT)
        return false;

    if(m_nKnum > 2){
        double dp;
        spline_quadratic_val(m_nKnum, m_pT, m_pKnot, t, &value, &dp);

        return true;
    }else if(m_nKnum > 1){
        double dp;
        spline_linear_val(m_nKnum, m_pT, m_pKnot, t, &value, &dp);

        return true;
    }else{
        return false;
    }
}


//////////////////////////////////////////////////////////////////////////
LinearSpline::LinearSpline(const SSpline &spline_data)
    : Curve(spline_data){

}

LinearSpline::~LinearSpline(void){

}

bool LinearSpline::getValue(double t, double &value){
    if(!m_pT || m_nKnum < 2)
        return false;

    double rate = (t - m_pT[0]) / (m_pT[1] - m_pT[0]);
    if(rate < 0)
        value = m_pKnot[0];
    if(rate > 1)
        value = m_pKnot[1];
    value = (1 - rate) * m_pKnot[0] + rate * m_pKnot[1];

    return true;
}

