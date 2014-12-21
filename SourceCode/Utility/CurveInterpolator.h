#ifndef BEYOND_ENGINE_UTILITY_CURVEINTERPOLATOR_H__INCLUDE
#define BEYOND_ENGINE_UTILITY_CURVEINTERPOLATOR_H__INCLUDE

template<typename T>
struct CInterpolatePolicy
{
    static void Interpolate(const T& t1, const T& t2, float progress, T& result)
    {
        result = (t2 - t1) * progress + t1;
    }
};

// Added more implementation as we need. like Quat
template <typename T>
class CCurveInterpolator
{
public:
    CCurveInterpolator()
    {
    }

    ~CCurveInterpolator()
    {
    }

    void AddControl(const T& value)
    {
        m_controlList.push_back(value);
    }

    void UpdateControl(uint32_t index, const T& value)
    {
        BEATS_ASSERT(index < m_controlList.size(), "invalid index");
        m_controlList[index] = value;
    }

    void Clean()
    {
        m_controlList.clear();
    }

    const T& GetControl(uint32_t index) const
    {
        return m_controlList[index];
    }

    void Interpolate(float progress, T& result) const
    {
        BEATS_ASSERT(m_controlList.size() >= 0, "need more than 1 control");
        progress = glm::clamp(0.0f, 1.0f, progress);
		std::vector<T> intermediateResults = m_controlList;
		// interpolate base on intermeidateResults till finish
        do
        {
            for (uint32_t n = 1; n < intermediateResults.size(); ++n)
            {
                CInterpolatePolicy<T>::Interpolate(intermediateResults[n - 1], intermediateResults[n], progress, intermediateResults[n - 1]);
            }
			intermediateResults.resize(intermediateResults.size() - 1);
        } while (intermediateResults.size() > 1);
        result = intermediateResults[0];
    }

    uint32_t GetSize()
    {
        return m_controlList.size();
    }

private:
    std::vector<T> m_controlList;
};

#endif
