#include "stdafx.h"
#include "VMDConverter.h"

VMDConverter::VMDConverter()
{
}


VMDConverter::~VMDConverter()
{
}

bool VMDConverter::IsLinear(std::vector<BYTE> interpolation, int type)
{
	byte ax = interpolation[0 * 8 + type];
	byte ay = interpolation[0 * 8 + 4 + type];
	byte bx = interpolation[1 * 8 + type];
	byte by = interpolation[1 * 8 + 4 + type];
	return (ax == ay) && (bx == by);
}

glm::vec2 VMDConverter::GetBezierHandle(std::vector<BYTE> interpolation, int type, int ab)
{
	glm::vec2 bezierHandle((float)interpolation[ab*8+type], (float)interpolation[ab*8+4+type]);
	return bezierHandle / 127.0f;
}

glm::vec2 VMDConverter::SampleBezier(glm::vec2 bezierHandleA, glm::vec2 bezierHandleB, float t)
{
	glm::vec2 p0(0.0f);
	glm::vec2 p1 = bezierHandleA;
	glm::vec2 p2 = bezierHandleB;
	glm::vec2 p3(1.0f);

	glm::vec2 q0 = glm::mix(p0, p1, t);
	glm::vec2 q1 = glm::mix(p1, p2, t);
	glm::vec2 q2 = glm::mix(p2, p3, t);

	glm::vec2 r0 = glm::mix(q0, p1, t);
	glm::vec2 r1 = glm::mix(q1, q2, t);
	
	glm::vec2 s0 = glm::mix(r0, r1, t);

	return s0;
}

VMDConverter::FloatKeyFrame VMDConverter::FloatKeyFrame::Lerp(FloatKeyFrame from, FloatKeyFrame to, glm::vec2 t)
{
	return FloatKeyFrame(glm::mix<float>(from.Time, to.Time, t.x), glm::mix<float>(from.Value, to.Value, t.y));
}

void VMDConverter::FloatKeyFrame::AddBezierKeyFrames(std::vector<BYTE> interpolation, int type, 
	FloatKeyFrame* prevKeyframe, FloatKeyFrame& curKeyFrame, int interpolationQuality,
	std::vector<FloatKeyFrame>& keyframes, int& index)
{
	if (prevKeyframe == NULL || IsLinear(interpolation, type))
	{
		keyframes[index++] = curKeyFrame;
	}
	else
	{
		glm::vec2 bezierHandleA = GetBezierHandle(interpolation, type, 0);
		glm::vec2 bezierHandleB = GetBezierHandle(interpolation, type, 1);
		int sampleCount = interpolationQuality;
		for (int i = 0; i < sampleCount; ++i)
		{
			float t = (i + 1) / (float) sampleCount;
			glm::vec2 sample = SampleBezier(bezierHandleA, bezierHandleB, t);
			keyframes[index++] = FloatKeyFrame::Lerp(*prevKeyframe, curKeyFrame, sample);
		}
	}

}


VMDConverter::QuaternionKeyFrame VMDConverter::QuaternionKeyFrame::Lerp(QuaternionKeyFrame from, QuaternionKeyFrame to, glm::vec2 t)
{
	return QuaternionKeyFrame(glm::mix<float>(from.Time, to.Time, t.x), glm::slerp(from.Value, to.Value, t.y));
}

void VMDConverter::QuaternionKeyFrame::AddBezierKeyFrames(std::vector<BYTE> interpolation, int type, QuaternionKeyFrame* prevKeyframe, QuaternionKeyFrame& curKeyFrame, int interpolationQuality, std::vector<QuaternionKeyFrame>& keyframes, int& index)
{
	if (prevKeyframe == NULL || IsLinear(interpolation, type))
	{
		keyframes[index++] = curKeyFrame;
	}
	else
	{
		glm::vec2 BezierHandleA = GetBezierHandle(interpolation, type, 0);
		glm::vec2 BezierHandleB = GetBezierHandle(interpolation, type, 1);
		int sampleCount = interpolationQuality;
		for (int i = 0; i < sampleCount; ++i)
		{
			float t = (i + 1) / (float) sampleCount;
			glm::vec2 sample = SampleBezier(BezierHandleA, BezierHandleB, t);
			keyframes[index++] = QuaternionKeyFrame::Lerp(*prevKeyframe, curKeyFrame, sample);
		}
	}
}
