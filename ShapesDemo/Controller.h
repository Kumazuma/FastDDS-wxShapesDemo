#pragma once
#include <wx/wx.h>
#include <map>
#include <shared_mutex>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include "Model.h"
#include "Subscriber.h"

class wxDdsEvent;
class Controller: public wxEvtHandler, public eprosima::fastdds::dds::DataReaderListener
{
public:
	Controller();
	void Init(int domainId);

	std::optional<eprosima::fastrtps::rtps::GUID_t> CreateDataWriter(
		ShapeKind shapeKind,
		const eprosima::fastdds::dds::DataWriterQos& qos,
		uint8_t partitions, const wxString& color);

	std::optional<eprosima::fastrtps::rtps::GUID_t> CreateDataReader(
		ShapeKind shapeKind,
		const eprosima::fastdds::dds::DataReaderQos& qos,
		uint8_t partitions,
		bool isContentFiltered,
		bool useTake);

	void Update(float timeDelta);

	void Shutdown();

	void on_data_available(eprosima::fastdds::dds::DataReader* reader) override;

protected:
	void OnReceivedSamples(wxDdsEvent&);

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
	std::shared_mutex m_lockReaders;
	std::mutex m_lockSamples;
};

class wxDdsEvent: public wxThreadEvent
{
public:
	wxDdsEvent(wxEventType eventType, wxWindowID id = wxID_ANY)
		: wxThreadEvent(eventType, id)
	, m_relatedEntityGuid{}
	{
		
	}

	const eprosima::fastrtps::rtps::GUID_t& GetRelatedEntityGuid() const { return m_relatedEntityGuid; }
	void SetRelatedEntityGuid(const eprosima::fastrtps::rtps::GUID_t& guid)
	{
		m_relatedEntityGuid = guid;
	}
private:
	eprosima::fastrtps::rtps::GUID_t m_relatedEntityGuid;

};

wxDECLARE_EVENT(wxEVT_RECEIVED_SAMPLES, wxDdsEvent);
