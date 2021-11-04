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

	namespace InstrumentorUtils {
		template <size_t N>
		struct changeResult {
			char data[N];
		};

		template <size_t N, size_t K>
		constexpr auto cleanupOutputString(const char(&expr)[N], const char(&remove)[K]) {
			changeResult<N> result = {};

			size_t srcIndex = 0;
			size_t dstIndex = 0;

			while (srcIndex < N) {
				size_t matchIndex = 0;

				while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && expr[srcIndex + matchIndex] == remove[matchIndex])
					matchIndex++;

				if (matchIndex == K - 1)
					srcIndex += matchIndex;

				result.data[dstIndex++] = expr[srcIndex] == '"' ? '\'' : expr[srcIndex];
				srcIndex++;
			}

			return result;
		}
	}
}