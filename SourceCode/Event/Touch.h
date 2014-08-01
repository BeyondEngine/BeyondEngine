#ifndef BEYOND_ENGINE_EVENT_TOUCH_H_INCLUDE
#define BEYOND_ENGINE_EVENT_TOUCH_H_INCLUDE

#define INVALID_TOUCH_ID -1

class CTouch
{
public:
    CTouch( int id);
    ~CTouch();
    CTouch( CTouch&& other );

    CTouch& operator=( CTouch&& other );
public:
    int ID() const;

    void UpdateTouchInfo( int id, float x, float y );
    
    void SetTapCount(unsigned int uTapCount);
    unsigned int GetTapCount() const;

    //all point is the device touch location coordinates
    const kmVec2& GetStartPoint() const;
    const kmVec2& GetCurrentPoint() const;
    const kmVec2& GetPreviousPoint() const;
    float GetDelta() const;

    const kmVec2& GetDeltaXY();
private:
    bool m_bStart;
    int m_id;
    size_t m_uTapCount;
    kmVec2 m_ve2StartPoint;
    kmVec2 m_ve2Point;
    kmVec2 m_ve2PreviousPoint;
    kmVec2 m_deltaXY;
};

#endif//TOUCH_H_INCLUDE