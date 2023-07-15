#include "Subscriber.h"

Subscriber::Subscriber(ShapeKind shapeKind, eprosima::fastdds::dds::DataReader* pDataReader, bool useTake)
	: m_shapeKind(shapeKind)
	, m_pDataReader(pDataReader)
{
	setUseTake(useTake);
}

Subscriber::~Subscriber()
{
}

bool Subscriber::useTake() const
{
	return m_getFunc == &eprosima::fastdds::dds::DataReader::take;
}

void Subscriber::setUseTake(bool useTake)
{
	m_getFunc = useTake ? &eprosima::fastdds::dds::DataReader::take : &eprosima::fastdds::dds::DataReader::read;
}

ReturnCode_t Subscriber::GetSamples(eprosima::fastdds::dds::LoanableCollection& data_values,
	eprosima::fastdds::dds::SampleInfoSeq& sample_infos)
{
	return (m_pDataReader->*m_getFunc)(data_values, sample_infos, eprosima::fastdds::dds::LENGTH_UNLIMITED, eprosima::fastdds::dds::ANY_SAMPLE_STATE, eprosima::fastdds::dds::ANY_VIEW_STATE, eprosima::fastdds::dds::ANY_INSTANCE_STATE);
}

void Subscriber::ReturnLoan(eprosima::fastdds::dds::LoanableCollection& data_values,
	eprosima::fastdds::dds::SampleInfoSeq& sample_infos)
{
	m_pDataReader->return_loan(data_values, sample_infos);
}

eprosima::fastdds::dds::InstanceHandle_t Subscriber::GetInstanceHandle() const
{
	return m_pDataReader->get_instance_handle();
}
