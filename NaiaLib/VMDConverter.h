#pragma once
#include "precompiled.h"

class VMDConverter
{
public:

	//------------------------------------------------------------------------
	//	CustomKeyFrame
	//------------------------------------------------------------------------
	template<typename T>
	class CustomKeyFrame
	{
	protected:
		CustomKeyFrame(float time, T value) : Value(value), Time(time) { }

	public:
		float Time;
		T Value;
	};

	//------------------------------------------------------------------------
	//	FloatKeyFrame
	//------------------------------------------------------------------------
	class FloatKeyFrame : public CustomKeyFrame<float>
	{
	public:
		FloatKeyFrame(float time, float value) : CustomKeyFrame(time, value) {}

		static FloatKeyFrame Lerp(FloatKeyFrame from, FloatKeyFrame to, glm::vec2 t);
		static void AddBezierKeyFrames(std::vector<BYTE> interpolation, int type,
			FloatKeyFrame* prevKeyframe, FloatKeyFrame& curKeyFrame, int interpolationQuality,
			std::vector<FloatKeyFrame>& keyframes, int& index);
	};

	//------------------------------------------------------------------------
	//	QuaternionKeyFrame
	//------------------------------------------------------------------------
	class QuaternionKeyFrame : public CustomKeyFrame<glm::quat>
	{
	public:
		QuaternionKeyFrame(float time, glm::quat value) : CustomKeyFrame(time, value) { }

		static QuaternionKeyFrame Lerp(QuaternionKeyFrame from, QuaternionKeyFrame to, glm::vec2 t);
		static void AddBezierKeyFrames(std::vector<BYTE> interpolation, int type,
			QuaternionKeyFrame* prevKeyframe, QuaternionKeyFrame& curKeyFrame, int interpolationQuality,
			std::vector<QuaternionKeyFrame>& keyframes, int& index);
		float Mod360(float angle) { return (angle < 0) ? (angle + 360.0f) : (angle); }
	};

public:
	VMDConverter();
	~VMDConverter();

	static glm::vec2 GetBezierHandle(std::vector<BYTE> interpolation, int type, int ab);
	static bool IsLinear(std::vector<BYTE> interpolation, int type);
	static glm::vec2 SampleBezier(glm::vec2 bezierHandleA, glm::vec2 bezierHandleB, float t);
};

