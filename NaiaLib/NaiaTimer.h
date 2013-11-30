#pragma once
class Time
{
public:
	Time();
	Time(const Time& other);
	~Time();

	bool Initialize();
	void Update();

	float DeltaTime();
	
private:
	INT64 m_frequency;
	float m_ticksPerMs;
	INT64 m_startTime;
	float m_frameTime;
};

