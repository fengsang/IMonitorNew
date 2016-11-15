
//获取相对时间
class CRelativeTime
{
public:
	BOOL ResetRelativeTime();		//重新开始计时
	double GetRelativeTime();		//结束计时
public:
	CRelativeTime();
	~CRelativeTime();
private:
	LARGE_INTEGER m_frequency;
	LARGE_INTEGER m_startPerformanceCount;
	LARGE_INTEGER m_endPerformanceCount;
};