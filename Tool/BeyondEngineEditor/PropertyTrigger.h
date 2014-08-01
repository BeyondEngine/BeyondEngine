#ifndef BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_PROPERTYTRIGGER_H__INCLUDE
#define BEYOND_ENGINE_EDITOR_WXUICONTROL_WXWIDGETSPROPERTY_PROPERTYTRIGGER_H__INCLUDE

class CWxwidgetsPropertyBase;
class CComponentProxy;

// NOTICE: Keep the order: longer string is at the front, so we will check >= before >.
enum ECompareOperator
{
    eCO_GreaterEqual,
    eCO_Equal,
    eCO_LessEqual,
    eCO_NotEqaul,
    eCO_Greater,
    eCO_Less,

    eCO_Count,
    eCO_Force32Bit = 0xFFFFFFFF
};

const static TCHAR* CompareOperatorString[] =
{
    _T(">="),
    _T("=="),
    _T("<="),
    _T("!="),
    _T(">"),
    _T("<"),

    _T("Count"),
};

struct STriggerContent
{
    STriggerContent(const TString& strProperty, ECompareOperator op, const TString& refValue);
    ~STriggerContent();
    bool IsOk(CComponentProxy* pComponent);
    const TString& GetPropertyName()const {return m_strPropertyName;};
private:
    template<typename T>
    bool ExamByOperator(const T& t1, const T& t2)
    {
        bool bRet = false;
        switch (m_op)
        {
        case eCO_Greater:
            bRet = t1 > t2;
            break;
        case eCO_GreaterEqual:
            bRet = t1 >= t2;
            break;
        case eCO_Less:
            bRet = t1 < t2;
            break;
        case eCO_LessEqual:
            bRet = t1 <= t2;
            break;
        case eCO_NotEqaul:
            bRet = t1 != t2;
            break;
        case eCO_Equal:
            bRet = t1 == t2;
            break;
        default:
            BEATS_ASSERT(false, _T("Invalid operator %d"), m_op);
            break;
        }
        return bRet;
    }

    ECompareOperator m_op;
    TString m_strPropertyName;
    TString m_strRefValue;
};

class CPropertyTrigger
{
public:
    CPropertyTrigger(const TString& strRawString);
    ~CPropertyTrigger();

    bool IsOk(CComponentProxy* pComponent);
    void Initialize();
    bool IsInitialized();
    const std::vector<STriggerContent*>& GetContent() const;

private:
    enum EAbstractRelation
    {
        eAR_True,
        eAR_False,
        eAR_Or,
        eAR_And,
        eAR_Count,
        eAR_Force32Bit = 0xFFFFFFFF
    };

    void ConvertExpression(const TString& str, std::vector<TString>& result);// By Reverse polishnotation.
    STriggerContent* GenerateContent(const TString& str);

private:
    TString m_rawString;
    std::vector<STriggerContent*> m_content;
};

#endif