
//��ȡ���ʱ��
class CRelativeTime
{
public:
	BOOL ResetRelativeTime();		//���¿�ʼ��ʱ
	double GetRelativeTime();		//������ʱ
public:
	CRelativeTime();
	~CRelativeTime();
private:
	LARGE_INTEGER m_frequency;
	LARGE_INTEGER m_startPerformanceCount;
	LARGE_INTEGER m_endPerformanceCount;
};