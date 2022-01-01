#include "engpch.h"
#include "Instrumentor.h"

namespace Engine
{
	void Instrumentor::BeginSession(const std::string& name, const std::string& filepath)
	{
		std::lock_guard lock(m_mutex);

		if (m_currentSession)
		{
			if (Log::GetCoreLogger())
			{
				ENG_CORE_ERROR("Instrumentator::BeginSession('{0}') when session '{1}' already open.", name, m_currentSession->Name);
			}

			InternalEndSession();
		}

		m_outputStream.open(filepath);

		if (m_outputStream.is_open())
		{
			m_currentSession = new InstrumentationSession({ name });
			WriteHeader();
		} else
		{
			if (Log::GetCoreLogger())
			{
				ENG_CORE_ERROR("Instrumentor could not open results file '{0}'.", filepath);
			}
		}
	}

	void Instrumentor::EndSession()
	{
		std::lock_guard lock(m_mutex);
		InternalEndSession();
	}

	void Instrumentor::InternalEndSession()
	{
		if (m_currentSession)
		{
			WriteFooter();
			m_outputStream.close();

			delete m_currentSession;
			m_currentSession = nullptr;
		}
	}

	void Instrumentor::WriteProfile(const ProfileResult& result)
	{
		std::stringstream json;

		json << std::setprecision(3) << std::fixed;
		json << ",{";
		json << "\"cat\":\"function\",";
		json << "\"dur\":" << (result.ElapsedTime.count()) << ',';
		json << "\"name\":\"" << result.Name << "\",";
		json << "\"ph\":\"X\",";
		json << "\"pid\":0,";
		json << "\"tid\":" << result.ThreadID << ",";
		json << "\"ts\":" << result.Start.count();
		json << "}";

		std::lock_guard lock(m_mutex);

		if (m_currentSession)
		{
			m_outputStream << json.str();
			m_outputStream.flush();
		}
	}

	void Instrumentor::WriteHeader()
	{
		m_outputStream << "{\"otherData\": {},\"traceEvents\":[{}";
		m_outputStream.flush();
	}

	void Instrumentor::WriteFooter()
	{
		m_outputStream << "]}";
		m_outputStream.flush();
	}

	Instrumentor& Instrumentor::Get()
	{
		static Instrumentor instance;
		return instance;
	}

	InstrumentationTimer::InstrumentationTimer(const char* name) : m_name(name), m_stopped(false)
	{
		m_startTimepoint = std::chrono::steady_clock::now();
	}

	InstrumentationTimer::~InstrumentationTimer()
	{
		if (!m_stopped)
			Stop();
	}

	void InstrumentationTimer::Stop()
	{
		auto endTimepoint = std::chrono::steady_clock::now();

		auto highResStart = floatingPointMicroseconds{ m_startTimepoint.time_since_epoch() };
		auto elapsedTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch()
			- std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimepoint).time_since_epoch();

		Instrumentor::Get().WriteProfile({ m_name, highResStart, elapsedTime, std::this_thread::get_id() });

		m_stopped = true;
	}
}
