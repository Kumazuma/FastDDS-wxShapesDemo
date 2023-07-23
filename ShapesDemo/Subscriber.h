#pragma once
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <atomic>
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
	void SetChanged(bool changed) { m_changed = changed;}
	bool IsChanged() const { return m_changed; }
	bool TrySetUnchanged()
	{
		bool expected = true;
		return m_changed.compare_exchange_strong(expected, false);
	}

private:
	ShapeKind m_shapeKind;
	eprosima::fastdds::dds::DataReader* m_pDataReader;
	DataReaderGetFunc m_getFunc;
	std::atomic_bool m_changed;
};
