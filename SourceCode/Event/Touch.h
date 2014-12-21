#ifndef BEYOND_ENGINE_EVENT_TOUCH_H_INCLUDE
#define BEYOND_ENGINE_EVENT_TOUCH_H_INCLUDE

#define INVALID_TOUCH_ID -1

class CTouch
{
public:
    CTouch(size_t id);
    ~CTouch();
    CTouch( CTouch&& other );

    CTouch& operator=( CTouch&& other );
public:
    size_t ID() const;
    void UpdateTouchInfo(size_t id, float x, float y);
    
    void SetTapCount(unsigned int uTapCount);
    unsigned int GetTapCount() const;

    //all point is the device touch location coordinates
    const CVec2& GetStartPoint() const;
    const CVec2& GetCurrentPoint() const;
    const CVec2& GetPreviousPoint() const;
    float GetDelta() const;

    const CVec2& GetDeltaXY();
    void Reset(size_t uID);

private:
    bool m_bStart;
    size_t m_id;
    uint32_t m_uTapCount;
    CVec2 m_ve2StartPoint;
    CVec2 m_ve2Point;
    CVec2 m_ve2PreviousPoint;
    CVec2 m_deltaXY;
};

#endif//TOUCH_H_INCLUDE