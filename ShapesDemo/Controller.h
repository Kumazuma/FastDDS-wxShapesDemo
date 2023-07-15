#pragma once
#include <wx/wx.h>
#include <map>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include "Model.h"
#include "Subscriber.h"

class Controller
{
public:
	void Init(int domainId);
	
	void CreateDataWriter(
		ShapeKind shapeKind,
		const eprosima::fastdds::dds::DataWriterQos& qos,
		uint8_t partitions, const wxString& color);

	void CreateDataReader(
		ShapeKind shapeKind,
		const eprosima::fastdds::dds::DataReaderQos& qos,
		uint8_t partitions,
		bool isContentFiltered,
		bool useTake);

	void Update(float timeDelta);

	void Shutdown();
private:
	eprosima::fastdds::dds::DomainParticipant* m_pDomainParticipant;
	eprosima::fastdds::dds::TypeSupport m_pTypeSupport;
	eprosima::fastdds::dds::Topic* m_pTopicCircle;
	eprosima::fastdds::dds::Topic* m_pTopicTriangle;
	eprosima::fastdds::dds::Topic* m_pTopicSquare;
	eprosima::fastrtps::rtps::GUID_t m_latestGrabbedGuid;
	std::unordered_map<uint8_t, eprosima::fastdds::dds::Publisher*> m_publisherTable;
	std::unordered_map<uint8_t, eprosima::fastdds::dds::Subscriber*> m_subscriberTable;
	std::vector<eprosima::fastdds::dds::DataWriter*> m_writers;
	std::vector<Subscriber*> m_readers;
};

//namespace std
//{
//	template<>
//	struct hash<eprosima::fastrtps::rtps::GUID_t>
//	{
//		using TKey = eprosima::fastrtps::rtps::GUID_t;
//		size_t operator()(const TKey& key) const noexcept
//		{
//			size_t ret = 0;
//			const auto size = reinterpret_cast<const size_t*>(&key);
//			for(int i = 0; i < sizeof(key) / sizeof(size_t); ++i)
//			{
//				ret ^= size[i];
//			}
//
//			return ret;
//		}
//	};
//}
