#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>

class AnimationCurve 
{
public:
    enum CurveType
    {
        Curve_CUBIC = 0,   // 三次样条
        Curve_LINEAR,      // 折线

        Curve_COUNT,
    };


    static std::shared_ptr<AnimationCurve> Linear(double x1, double y1, double x2, double y2);
    static std::shared_ptr<AnimationCurve> Cubic(double x1, double y1, double x2, double y2);
    AnimationCurve();   // 使用静态方法来构造
	~AnimationCurve(); 
    double Evaluate(double key);
    void AddKey(double key, double value);
	void MoveKey(unsigned int index, double value);
	void RemoveKey(unsigned int index);
    void ChangeType(CurveType type);
    CurveType GetCurveType();

    int Size();
    double Key(int index);
    void SetKey(int index, double val);
    double Value(int index);
    void SetValue(int index, double val);
private:
    void ReBuildDDP();
    CurveType curveType;
    double *m_ddp;

    std::vector<double> m_keys;
    std::vector<double> m_values;
};