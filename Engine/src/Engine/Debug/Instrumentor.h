#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <string>
#include <thread>

namespace Engine {
	using floatingPointMicroseconds = std::chrono::duration<double, std::micro>;

	struct ProfileResult {
		std::string name;
		floatingPointMicroseconds start;
		std::chrono::microseconds elapsedTime;
		std::thread::id threadID;
	};

	struct InstrumentationSession {
		std::string name;
	};

	class Instrumentor {
	public:
		Instrumentor();

		void beginSession(const std::string& name, const std::string& filepath = "results.json");
		void endSession();
		void internalEndSession();

		void writeProfile(const ProfileResult& result);
		void writeHeader();
		void writeFooter();

		static Instrumentor& get();

	private:
		InstrumentationSession* m_currentSession;
		std::mutex m_mutex;
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
		std::chrono::time_point<std::chrono::steady_clock> m_startTimepoint;
		bool m_stopped;
	};
}