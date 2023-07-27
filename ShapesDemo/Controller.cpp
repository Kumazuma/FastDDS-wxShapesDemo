#include "Controller.h"
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/topic/TypeSupport.hpp>
#include <array>
#include "Model.h"
#include "ShapePubSubTypes.h"
#include "ShapesDemo.h"
#include "Shape.h"

using namespace DirectX;
wxDEFINE_EVENT(wxEVT_RECEIVED_SAMPLES, wxDdsEvent);

Controller::Controller()
{
}

void Controller::Init(int domainId)
{
	auto dpf = eprosima::fastdds::dds::DomainParticipantFactory::get_instance();
	m_pTypeSupport = eprosima::fastdds::dds::TypeSupport(new ShapeTypePubSubType());
	m_pDomainParticipant = dpf->create_participant(domainId, dpf->get_default_participant_qos());
	m_pTypeSupport.register_type(m_pDomainParticipant);
	auto typeName = m_pTypeSupport->getName();
	auto topicQos = m_pDomainParticipant->get_default_topic_qos();
	m_pTopicCircle = m_pDomainParticipant->create_topic("Circle", typeName, topicQos);
	m_pTopicTriangle = m_pDomainParticipant->create_topic("Triangle", typeName, topicQos);
	m_pTopicSquare = m_pDomainParticipant->create_topic("Square", typeName, topicQos);
	Bind(wxEVT_RECEIVED_SAMPLES, &Controller::OnReceivedSamples, this);
}

std::optional<eprosima::fastrtps::rtps::GUID_t> Controller::CreateDataWriter(
	ShapeKind shapeKind, const eprosima::fastdds::dds::DataWriterQos& qos,
	uint8_t partitions, const wxString& color)
{
	eprosima::fastdds::dds::Publisher* pPublisher;
	auto it = m_publisherTable.find(partitions);
	if(it != m_publisherTable.end())
	{
		pPublisher = it->second;
	}
	else
	{
		auto pubQos = m_pDomainParticipant->get_default_publisher_qos();
		pubQos.entity_factory().autoenable_created_entities = true;
		if (partitions & 0x1 << 1)
			pubQos.partition().push_back("A");

		if (partitions & 0x1 << 2)
			pubQos.partition().push_back("B");

		if (partitions & 0x1 << 3)
			pubQos.partition().push_back("C");

		if (partitions & 0x1 << 4)
			pubQos.partition().push_back("D");

		if (partitions & 0x1 << 0)
			pubQos.partition().push_back("*");

		pPublisher =  m_pDomainParticipant->create_publisher(pubQos);
		m_publisherTable.emplace(partitions, pPublisher);
	}

	eprosima::fastdds::dds::Topic* pTopic;
	switch(shapeKind)
	{
	case ShapeKind::Circle:
		pTopic = m_pTopicCircle;
		break;

	case ShapeKind::Square:
		pTopic = m_pTopicSquare;
		break;

	case ShapeKind::Triangle:
		pTopic = m_pTopicTriangle;
		break;
	}

	auto dw = pPublisher->create_datawriter(pTopic, qos);
	m_writers.push_back(dw);
	auto instanceHandle = eprosima::fastrtps::rtps::iHandle2GUID(dw->get_instance_handle());
	auto& app = wxGetApp();
	auto& model = app.GetModel();
	SendingShape shape{ color, 30, {CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2}, {0.f, 0.f} };
	model.sendingShapes.emplace( instanceHandle, std::make_tuple(shapeKind, shape));
	auto it2 = model.entityInfoTable.emplace(instanceHandle, EntityInfo{});
	auto& entity = it2.first->second;
	entity.color = color;
	entity.entityType = wxS("Pub");
	entity.guid = instanceHandle;

	switch(shapeKind)
	{
	case ShapeKind::Circle:
		entity.topic = wxS("Circle");
		break;

	case ShapeKind::Square:
		entity.topic = wxS("Square");
		break;

	case ShapeKind::Triangle:
		entity.topic = wxS("Triangle");
		break;

	}

	entity.reliability = qos.reliability().kind == eprosima::fastdds::dds::BEST_EFFORT_RELIABILITY_QOS ?
		wxS("BEST_EFFORT") :
		wxS("RELIABLE");
	
	return instanceHandle;
}

std::optional<eprosima::fastrtps::rtps::GUID_t> Controller::CreateDataReader(ShapeKind shapeKind, const eprosima::fastdds::dds::DataReaderQos& qos, uint8_t partitions, bool isContentFiltered, bool useTake)
{
	eprosima::fastdds::dds::Subscriber* pSubscriber;
	auto it = m_subscriberTable.find(partitions);
	if (it != m_subscriberTable.end())
	{
		pSubscriber = it->second;
	}
	else
	{
		auto subQos = m_pDomainParticipant->get_default_subscriber_qos();
		subQos.entity_factory().autoenable_created_entities = true;
		if (partitions & 0x1 << 1)
			subQos.partition().push_back("A");

		if (partitions & 0x1 << 2)
			subQos.partition().push_back("B");

		if (partitions & 0x1 << 3)
			subQos.partition().push_back("C");

		if (partitions & 0x1 << 4)
			subQos.partition().push_back("D");

		if (partitions & 0x1 << 0)
			subQos.partition().push_back("*");

		pSubscriber = m_pDomainParticipant->create_subscriber(subQos);
		m_subscriberTable.emplace(partitions, pSubscriber);
	}

	eprosima::fastdds::dds::Topic* pTopic;
	switch (shapeKind)
	{
	case ShapeKind::Circle:
		pTopic = m_pTopicCircle;
		break;

	case ShapeKind::Square:
		pTopic = m_pTopicSquare;
		break;

	case ShapeKind::Triangle:
		pTopic = m_pTopicTriangle;
		break;
	}

	auto dr = pSubscriber->create_datareader(pTopic, qos, this, eprosima::fastdds::dds::StatusMask::data_available());
	m_readers.push_back(new Subscriber(shapeKind, dr, useTake));
	auto instanceHandle = eprosima::fastrtps::rtps::iHandle2GUID(dr->get_instance_handle());
	auto& app = wxGetApp();
	auto& model = app.GetModel();
	model.shapeListTable.emplace(instanceHandle, std::make_tuple(shapeKind, std::vector<ReceivedShape>{ }));
	auto it2 = model.entityInfoTable.emplace(instanceHandle, EntityInfo{});
	auto& entity = it2.first->second;
	entity.color = wxS("*");
	entity.entityType = wxS("Sub");
	entity.guid = instanceHandle;

	switch(shapeKind)
	{
	case ShapeKind::Circle:
		entity.topic = wxS("Circle");
		break;

	case ShapeKind::Square:
		entity.topic = wxS("Square");
		break;

	case ShapeKind::Triangle:
		entity.topic = wxS("Triangle");
		break;

	}

	entity.reliability = qos.reliability().kind == eprosima::fastdds::dds::BEST_EFFORT_RELIABILITY_QOS ?
		wxS("BEST_EFFORT") :
		wxS("RELIABLE");
	
	return instanceHandle;
}

void Controller::Update(float timeDelta)
{
	auto& model = wxGetApp().GetModel();
	auto guid = model.grabbedShapeGuid;
	// Update
	XMVECTOR upVector = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMVECTOR downVector = XMVectorSet(0.f, -1.f, 0.f, 0.f);
	XMVECTOR leftVector = XMVectorSet(-1.f, 0.f, 0.f, 0.f);
	XMVECTOR rightVector = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	for(auto& pair: model.sendingShapes)
	{
		if(pair.first == guid || pair.first == m_latestGrabbedGuid)
			continue;

		auto& shape = std::get<1>(pair.second);
		XMVECTOR pos = XMLoadFloat2(&shape.pos);
		XMVECTOR power = XMLoadFloat2(&shape.power);
		const auto r = shape.size / 2;
		
		XMVECTOR boxVector = XMVectorSet(CANVAS_WIDTH / 2.f - r, CANVAS_HEIGHT / 2.f - r, 0.f, 0.f);
		XMVECTOR relativePos = XMVectorSubtract(pos, XMVectorSet(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, 0, 0));
		XMVECTOR moveDelta = timeDelta * power;
		pos = XMVectorAdd(pos, moveDelta);
		XMVECTOR plainVector{};
		// Check Intersect

		if(XMVectorGetY(relativePos) < -XMVectorGetY(boxVector))
		{
			plainVector = XMVectorAdd(plainVector, upVector);
		}
		
		if(XMVectorGetY(relativePos) > XMVectorGetY(boxVector))
		{
			plainVector = XMVectorAdd(plainVector, downVector);
		}

		if(XMVectorGetX(relativePos) < -XMVectorGetX(boxVector))
		{
			plainVector = XMVectorAdd(plainVector, rightVector);
		}

		if(XMVectorGetX(relativePos) > XMVectorGetX(boxVector))
		{
			plainVector = XMVectorAdd(plainVector, leftVector);
		}

		// 반사되는 곳이 없으면 따로 이동도 하지 않는다.
		if(XMVector2Length(plainVector).m128_f32[0] > 0.00001f)
		{
			XMVECTOR direction = XMVector2Normalize(power);
			XMVECTOR prevPos = relativePos;
			if(XMVectorGetY(relativePos) < -XMVectorGetY(boxVector))
			{
				XMVECTOR d1 = XMVector2Dot(boxVector, downVector);
				XMVECTOR d2 = XMVector2Dot(relativePos, upVector);
				XMVECTOR d3 = XMVectorSubtract(d1, d2);
				XMVECTOR d4 = XMVector2Dot(direction, upVector);
				XMVECTOR d5 = XMVectorDivide(d3, d4);
				XMVECTOR d = XMVectorMultiply(d5, direction);
				relativePos = XMVectorAdd(relativePos, d);
			}

			if(XMVectorGetY(relativePos) > XMVectorGetY(boxVector))
			{
				XMVECTOR d1 = XMVector2Dot(boxVector, upVector);
				XMVECTOR d2 = XMVector2Dot(relativePos, upVector);
				XMVECTOR d3 = XMVectorSubtract(d1, d2);
				XMVECTOR d4 = XMVector2Dot(direction, upVector);
				XMVECTOR d5 = XMVectorDivide(d3, d4);
				XMVECTOR d = XMVectorMultiply(d5, direction);
				relativePos = XMVectorAdd(relativePos, d);
			}

			if(XMVectorGetX(relativePos) < -XMVectorGetX(boxVector))
			{
				XMVECTOR d1 = XMVector2Dot(boxVector, leftVector);
				XMVECTOR d2 = XMVector2Dot(relativePos, rightVector);
				XMVECTOR d3 = XMVectorSubtract(d1, d2);
				XMVECTOR d4 = XMVector2Dot(direction, rightVector);
				XMVECTOR d5 = XMVectorDivide(d3, d4);
				XMVECTOR d = XMVectorMultiply(d5, direction);
				relativePos = XMVectorAdd(relativePos, d);
			}

			if(XMVectorGetX(relativePos) > XMVectorGetX(boxVector))
			{
				XMVECTOR d1 = XMVector2Dot(boxVector, rightVector);
				XMVECTOR d2 = XMVector2Dot(relativePos, rightVector);
				XMVECTOR d3 = XMVectorSubtract(d1, d2);
				XMVECTOR d4 = XMVector2Dot(direction, rightVector);
				XMVECTOR d5 = XMVectorDivide(d3, d4);
				XMVECTOR d = XMVectorMultiply(d5, direction);
				relativePos = XMVectorAdd(relativePos, d);
			}
	
			plainVector = XMVector2Normalize(plainVector);
			power = XMVector2Reflect(power, plainVector);
			direction = XMVector2Normalize(power);
			relativePos = XMVectorAdd(relativePos, XMVectorMultiply(direction, XMVector2Length(XMVectorSubtract(relativePos, prevPos))));
			pos = XMVectorAdd(XMVectorSet(CANVAS_WIDTH / 2, CANVAS_HEIGHT / 2, 0, 0), relativePos);

		}

		XMStoreFloat2(&shape.pos, pos);
		if(XMVectorGetX(XMVector2Length(power)) > 720)
		{
			power = XMVectorScale(XMVector2Normalize(power), 720);
		}

		XMStoreFloat2(&shape.power, power);
	}

	if(m_latestGrabbedGuid != eprosima::fastrtps::rtps::GUID_t::unknown() && guid == eprosima::fastrtps::rtps::GUID_t::unknown())
	{
		guid = m_latestGrabbedGuid;
		m_latestGrabbedGuid = {};
		auto it = model.sendingShapes.find(guid);
		if(it != model.sendingShapes.end())
		{
			auto& shape = std::get<1>(it->second);
			XMVECTOR pos = XMLoadFloat2(&shape.pos);
			XMVECTOR prevPos = XMLoadFloat2(&model.prevPosition);
			XMVECTOR diff = XMVectorSubtract(pos, prevPos);
			XMVECTOR power = XMVectorScale(diff, 1.f / 0.016f);
			XMStoreFloat2(&shape.power, power);
		}
	}
	else if(guid != eprosima::fastrtps::rtps::GUID_t::unknown())
	{
		m_latestGrabbedGuid = guid;
	}

	{
		auto it = model.sendingShapes.find(m_latestGrabbedGuid);
		if(it != model.sendingShapes.end())
		{
			auto& shape = std::get<1>(it->second);
			
			model.prevPosition = shape.pos;
		}
		else
		{
			m_latestGrabbedGuid = {};
		}
	}

	// Send
	for(auto writer: m_writers)
	{
		auto guid = eprosima::fastrtps::rtps::iHandle2GUID(writer->get_instance_handle());
		auto& shape = std::get<1>(model.sendingShapes[guid]);
		ShapeType msg;
		msg.color(shape.rgb.utf8_string());
		msg.shapesize(shape.size);
		msg.x(shape.pos.x);
		msg.y(shape.pos.y);
		writer->write(&msg);
	}
}

void Controller::Shutdown()
{
	for(auto reader: m_readers)
	{
		delete reader;
	}

	m_pDomainParticipant->delete_contained_entities();
	m_pDomainParticipant->delete_topic(m_pTopicTriangle);
	m_pDomainParticipant->delete_topic(m_pTopicCircle);
	m_pDomainParticipant->delete_topic(m_pTopicSquare);
	auto dpf = eprosima::fastdds::dds::DomainParticipantFactory::get_instance();
	dpf->delete_participant(m_pDomainParticipant);
}

void Controller::on_data_available(eprosima::fastdds::dds::DataReader* reader)
{
	auto& guid = reader->guid();
	std::vector<ReceivedShape> shapes;
	{
		std::shared_lock<std::shared_mutex> guard{m_lockReaders};
		auto it = std::find_if(m_readers.begin(), m_readers.end(), [guid](Subscriber* sub) {return guid == sub->GetInstanceHandle();});
		if(it == m_readers.end())
			return;

		auto* sub = *it;
		// Receive
		eprosima::fastdds::dds::LoanableSequence<::ShapeType> sequence(4096);
		eprosima::fastdds::dds::SampleInfoSeq sampleInfoSeq(4096);
		auto ret = sub->GetSamples(sequence, sampleInfoSeq);
		if(ret == ReturnCode_t::RETCODE_OK)
		{
			const auto count = sequence.length();
			shapes.reserve(count);
			for(int i = 0; i < count; ++i)
			{
				auto& sampleInfo = sampleInfoSeq[i];
				if(!sampleInfo.valid_data)
					continue;

				auto& data = sequence[i];
				shapes.emplace_back(ReceivedShape{wxString::FromUTF8(data.color()), data.shapesize(), {(float)data.x(), (float)data.y()}, eprosima::fastrtps::rtps::iHandle2GUID(sampleInfo.publication_handle)});
			}

			sub->ReturnLoan(sequence, sampleInfoSeq);
		}
	}

	auto* event = new wxDdsEvent(wxEVT_RECEIVED_SAMPLES);
	event->SetRelatedEntityGuid(reader->guid());
	event->SetPayload(std::shared_ptr<std::vector<ReceivedShape>>{new std::vector<ReceivedShape>{std::move(shapes)}});
	QueueEvent(event);
}

void Controller::OnReceivedSamples(wxDdsEvent& event)
{
	const auto& guid = event.GetRelatedEntityGuid();
	auto& app = wxGetApp();
	auto& model = app.GetModel();
	auto it = model.shapeListTable.find(guid);
	if(it == model.shapeListTable.end())
		return;

	auto list = event.GetPayload<std::shared_ptr<std::vector<ReceivedShape>>>();
	std::get<1>(it->second) = std::move(*list);
	app.UpdateView();
}
