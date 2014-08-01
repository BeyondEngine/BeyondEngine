#ifndef BEYOND_ENGINE_SPLINE_CURVE_H_INCLUDE
#define BEYOND_ENGINE_SPLINE_CURVE_H_INCLUDE

#include <string>
#include <vector>
#include <map>
#include <memory>

// spline data
struct SSpline
{
    struct Point
    {
        float t, y;

        Point(void) : t(0), y(0){}
        Point(float at, float ay) : t(at), y(ay){}
    };

    enum
    {
        SPLINE_CUBIC = 0,

        SPLINE_COUNT,
    };

    static std::string spline_name[SPLINE_COUNT];

    int type;

    typedef std::vector<Point> TKnotsList;
    std::string name;
    TKnotsList knots;

    bool bLimited;
    float fLimit_left;
    float fLimit_top;
    float fLimit_right;
    float fLimit_bottom;

    SSpline(void)
        : type(SPLINE_CUBIC), bLimited(false), fLimit_left(0.0f),
        fLimit_top(0.0f), fLimit_right(0.0f), fLimit_bottom(0.0f)
    {

    }

    SSpline(const std::string &spline_name)
        : type(SPLINE_CUBIC), name(spline_name), bLimited(false), fLimit_left(0.0f),
        fLimit_top(0.0f), fLimit_right(0.0f), fLimit_bottom(0.0f)
    {

    }

    void setKnots(const std::vector<std::pair<float, float> > &nots)
    {
        knots.clear();
        for(size_t i=0; i<nots.size(); ++i)
        {
            knots.push_back(Point(nots[i].first, nots[i].second));
        }
    }

    void addKnots(std::pair<float, float> knot)
    {
        TKnotsList::iterator it = knots.begin();
        for(; it!=knots.end(); ++it)
        {
            if(knot.first < (*it).t)
                break;
            else if(knot.first == (*it).t)
            {
                return ;
            }
        }
        knots.insert(it, Point(knot.first, knot.second));
    }

    int addKnots(Point knot)
    {
        int count = 0;
        TKnotsList::iterator it = knots.begin();
        for(; it!=knots.end(); ++it)
        {
            if(knot.t < (*it).t)
                break;
            else if(knot.t == (*it).t)
            {
                return -1;
            }
            ++count;
        }
        knots.insert(it, knot);
        return count;
    }

    void removeKnot(size_t index)
    {
        if(index >= 0 && index < knots.size())
        {
            auto itr = knots.begin();
            std::advance(itr, index);
            knots.erase(itr);
        }
    }

    void setKnot(size_t index, Point pt)
    {
        if(index >= 0 && index < knots.size())
        {
            knots[index] = pt;
        }
    }

    bool findKnot(Point pt, size_t &index)
    {
        bool bRet = false;
        for(size_t i = 0; i < knots.size(); ++i)
        {
            if(abs(pt.t - knots[i].t) < 4.f && abs(pt.y - knots[i].y) < 4.f)
            {
                index = i;
                bRet = true;
                break;
            }
        }
        return bRet;
    }

    void setLimit(float left, float top, float right, float bottom)
    {
        bLimited = true;
        fLimit_left = left;
        fLimit_top = top;
        fLimit_right = right;
        fLimit_bottom = bottom;
    }

    void removeLimit(void)
    {
        bLimited = false;
    }

    void setName(const std::string &name)
    {
        this->name = name;
    }

    size_t size(void) const{ return knots.size(); }
};


class Curve
{
protected:
    std::string m_strName;
    int m_nKnum;

    double *m_pT;
    double *m_pKnot;

public:
    Curve(const SSpline &spline_data);
    virtual ~Curve(void);

    std::string name(void) const{ return m_strName; }

    virtual bool getValue(double t, double &value) = 0;

};

class CubicSpline : public Curve
{
private:
    double *m_pDdp;

public:
    CubicSpline(const SSpline &spline_data);
    virtual ~CubicSpline(void);

    virtual bool getValue(double t, double &value);
};

class BSpline : public Curve
{
public:
    BSpline(const SSpline &spline_data);
    virtual ~BSpline(void);

    virtual bool getValue(double t, double &value);
};


class QuadraticSpline : public Curve
{

public:
    QuadraticSpline(const SSpline &spline_data);
    virtual ~QuadraticSpline(void);

    virtual bool getValue(double t, double &value);
};

class LinearSpline : public Curve
{

public:
    LinearSpline(const SSpline &spline_data);
    virtual ~LinearSpline(void);

    virtual bool getValue(double t, double &value);
};

class CurveFactory
{
private:
    CurveFactory(void);
    CurveFactory(const CurveFactory&);
public:
    static Curve *createCurve(const SSpline &spline_data){
        switch(spline_data.type){
        case SSpline::SPLINE_CUBIC:
            return new CubicSpline(spline_data);
        default:
            return nullptr;
        }
    }
};

#endif // __AASPLINE_H__

