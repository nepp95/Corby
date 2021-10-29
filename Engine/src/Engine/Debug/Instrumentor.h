#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>
#include <string>
#include <thread>

namespace Engine {
	struct ProfileResult {
		std::string name;
		long long start, end;
		uint32_t threadID;
	};

	struct InstrumentationSession {
		std::string name;
	};

	class Instrumentor {
	public:
		Instrumentor();

		void beginSession(const std::string& name, const std::string& filepath = "results.json");
		void endSession();

		void writeProfile(const ProfileResult& result);
		void writeHeader();
		void writeFooter();

		static Instrumentor& get();

	private:
		InstrumentationSession* m_currentSession;
		std::ofstream m_outputStream;
		int m_profileCount;
	};

	class InstrumentationTimer {
	public:
		InstrumentationTimer(const char* name);
		~InstrumentationTimer();

		void stop();

	private:
		const char* m_name;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimepoint;
		bool m_stopped;
	};
}