#include "spline.h"
#include "Curve.h"

std::shared_ptr<AnimationCurve> AnimationCurve::Linear(double x1, double y1, double x2, double y2)
{
    auto rv = std::make_shared<AnimationCurve>();
    rv->curveType = Curve_LINEAR;
    rv->m_keys.reserve(2);
    rv->m_keys.push_back(x1);
    rv->m_keys.push_back(x2);
    rv->m_values.reserve(2);
    rv->m_values.push_back(y1);
    rv->m_values.push_back(y2);

    return rv;
}

std::shared_ptr<AnimationCurve> AnimationCurve::Cubic(double x1, double y1, double x2, double y2)
{
    auto rv = std::make_shared<AnimationCurve>();
    rv->curveType = Curve_CUBIC;

    rv->m_keys.reserve(2);
    rv->m_keys.push_back(x1);
    rv->m_keys.push_back(x2);
    rv->m_values.reserve(2);
    rv->m_values.push_back(y1);
    rv->m_values.push_back(y2);

    return rv;
}

void AnimationCurve::AddKey(double key, double value)
{
    auto amount = m_keys.size();
    for (unsigned int i = 0; i < amount; ++i)
    {
        if (m_keys[i] > key)
        {
            m_keys.insert(m_keys.begin() + i, key);
            m_values.insert(m_values.begin() + i, value);
            break;
        }
        else if(i == amount - 1)
        {
            m_keys.push_back(key);
            m_values.push_back(value);
            break;
        }
    }
    ReBuildDDP();
}

double AnimationCurve::Evaluate(double key)
{
    double value = DBL_MAX;
    if (m_keys.back() >= key)
    {
        switch (curveType)
        {
        case AnimationCurve::Curve_CUBIC:
            {
                auto num = m_keys.size();
                if(num > 3)
                {
                    double dp, ddp;
                    value = spline_cubic_val(num, &m_keys[0], &m_values[0], m_ddp, key, &dp, &ddp);
                }
                else if(num == 3)
                {
                    double dp;
                    spline_quadratic_val(num, &m_keys[0], &m_values[0], key, &value, &dp);
                }
                else if(num == 2)
                {
                    double dp;
                    spline_linear_val(num, &m_keys[0], &m_values[0], key, &value, &dp);
                }
            }
            break;
        case AnimationCurve::Curve_LINEAR:
            {
                double dp;
                spline_linear_val(m_keys.size(), &m_keys[0], &m_values[0], key, &value, &dp);
            }
            break;
        default:
            break;
        }
    }

    return value;
}

void AnimationCurve::MoveKey(unsigned int index, double value)
{
    if (index > 0 && index < m_values.size())
    {
        m_values[index] = value;
    }
    else
    {
       // BOOST_ASSERT_MSG(false, "index 越界");
    }
    ReBuildDDP();
}

void AnimationCurve::RemoveKey(unsigned int index)
{
    if (index > 0 && index < m_keys.size())
    {
        m_keys.erase(m_keys.begin() + index);
        m_values.erase(m_values.begin() + index);
    }
    else
    {
       //(false, "index 越界");
    }
    ReBuildDDP();
}

void AnimationCurve::ReBuildDDP()
{
    if (curveType == Curve_CUBIC && m_keys.size() > 3)
    {
		if (m_ddp)
		{
			delete[]m_ddp;
			m_ddp = nullptr;
		}
        m_ddp = spline_cubic_set(m_keys.size(), &m_keys[0], &m_values[0], 2, 0, 2, 0);
    }
}

void AnimationCurve::ChangeType(CurveType type)
{
    curveType = type;
    ReBuildDDP();
}

int AnimationCurve::Size()
{
    return m_keys.size();
}

double AnimationCurve::Key(int index)
{
    return m_keys[index];
}

double AnimationCurve::Value(int index)
{
    return m_values[index];
}

void AnimationCurve::SetKey(int index, double val)
{
    m_keys[index] = val;
    ReBuildDDP();
}

void AnimationCurve::SetValue(int index, double val)
{
    m_values[index] = val;
    ReBuildDDP();
}

AnimationCurve::CurveType AnimationCurve::GetCurveType()
{
    return curveType;
}

AnimationCurve::AnimationCurve()
{
	m_ddp = NULL;
}

AnimationCurve::~AnimationCurve()
{
	if (m_ddp)
	{
		delete[]m_ddp;
	}
}
