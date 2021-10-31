#include "engpch.h"
#include "Instrumentor.h"

namespace Engine {
	Instrumentor::Instrumentor() : m_currentSession(nullptr) {
	}

	void Instrumentor::beginSession(const std::string& name, const std::string& filepath) {
		std::lock_guard lock(m_mutex);

		if (m_currentSession) {
			if (Log::getCoreLogger()) {
				ENG_CORE_ERROR("Instrumentator::BeginSession('{0}') when session '{1}' already open.", name, m_currentSession->name);
			}

			internalEndSession();
		}

		m_outputStream.open(filepath);

		if (m_outputStream.is_open()) {
			m_currentSession = new InstrumentationSession({ name });
			writeHeader();
		}
		else {
			if (Log::getCoreLogger()) {
				ENG_CORE_ERROR("Instrumentor could not open results file '{0}'.", filepath);
			}
		}
	}

	void Instrumentor::endSession() {
		std::lock_guard lock(m_mutex);
		internalEndSession();
	}

	void Instrumentor::internalEndSession() {
		if (m_currentSession) {
			writeFooter();
			m_outputStream.close();

			delete m_currentSession;
			m_currentSession = nullptr;
		}
	}

	void Instrumentor::writeProfile(const ProfileResult& result) {
		std::stringstream json;

		std::string name = result.name;
		std::replace(name.begin(), name.end(), '"', '\'');

		json << std::setprecision(3) << std::fixed;
		json << ",{";
		json << "\"cat\":\"function\",";
		json << "\"dur\":" << (result.elapsedTime.count()) << ',';
		json << "\"name\":\"" << name << "\",";
		json << "\"ph\":\"X\",";
		json << "\"pid\":0,";
		json << "\"tid\":" << result.threadID << ",";
		json << "\"ts\":" << result.start.count();
		json << "}";

		std::lock_guard lock(m_mutex);

		if (m_currentSession) {
			m_outputStream << json.str();
			m_outputStream.flush();
		}
	}

	void Instrumentor::writeHeader() {
		m_outputStream << "{\"otherData\": {},\"traceEvents\":[{}";
		m_outputStream.flush();
	}

	void Instrumentor::writeFooter() {
		m_outputStream << "]}";
		m_outputStream.flush();
	}

	Instrumentor& Instrumentor::get() {
		static Instrumentor instance;
		return instance;
	}

	InstrumentationTimer::InstrumentationTimer(const char* name) : m_name(name), m_stopped(false) {
		m_startTimepoint = std::chrono::steady_clock::now();
	}

	InstrumentationTimer::~InstrumentationTimer() {
		if (!m_stopped)
			stop();
	}

	void InstrumentationTimer::stop() {
		auto endTimepoint = std::chrono::steady_clock::now();

		auto highResStart = floatingPointMicroseconds{ m_startTimepoint.time_since_epoch() };
		auto elapsedTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch()
			- std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimepoint).time_since_epoch();

		Instrumentor::get().writeProfile({ m_name, highResStart, elapsedTime, std::this_thread::get_id() });

		m_stopped = true;
	}
}