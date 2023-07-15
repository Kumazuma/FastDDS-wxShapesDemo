#pragma once
#include <fastdds/dds/subscriber/DataReader.hpp>
#include "Model.h"

class Subscriber
{
	using DataReaderGetFunc = ReturnCode_t(eprosima::fastdds::dds::DataReader::*)(
		eprosima::fastdds::dds::LoanableCollection& data_values,
		eprosima::fastdds::dds::SampleInfoSeq& sample_infos,
		int32_t max_samples,
		eprosima::fastdds::dds::SampleStateMask sample_states,
		eprosima::fastdds::dds::ViewStateMask view_states,
		eprosima::fastdds::dds::InstanceStateMask instance_states);
public:
	Subscriber(ShapeKind shapeKind, eprosima::fastdds::dds::DataReader* pDataReader, bool useTake);
	~Subscriber();
	bool useTake() const;
	void setUseTake(bool useTake);
	ReturnCode_t GetSamples(eprosima::fastdds::dds::LoanableCollection& data_values, eprosima::fastdds::dds::SampleInfoSeq& sample_infos);
	void ReturnLoan(eprosima::fastdds::dds::LoanableCollection& data_values, eprosima::fastdds::dds::SampleInfoSeq& sample_infos);
	eprosima::fastdds::dds::InstanceHandle_t GetInstanceHandle() const;
private:
	ShapeKind m_shapeKind;
	eprosima::fastdds::dds::DataReader* m_pDataReader;
	DataReaderGetFunc m_getFunc;
};
