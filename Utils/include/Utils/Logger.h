#pragma once

#include <string>
#include <vector>

#include <fmt/core.h>
#include <fmt/color.h>

#include "imgui.h"

#define COLOR(str) fmt::color::str

namespace Util
{
	class Logger
	{
	public:
		template <class... ArgsType>
		inline static void Debug(std::string_view fmt, ArgsType &&...args)
		{
			if (enabled)
			{
				int oldSize = m_Buf.size();
				int color = 0xFFFFFFFF;
				
				std::string msg = fmt::format(fmt, std::forward<ArgsType>(args)...);
				msg = fmt::format("{}{}\n", "[Debug] ", msg);
				fmt::print(fg(fmt::color::white), "{}", msg);
				
				m_Buf.append(msg.c_str());
				AddOffset(oldSize, color);
			}
		}

		template <class... ArgsType>
		inline static void Info(std::string_view fmt, ArgsType &&...args)
		{
			if (enabled)
			{
				int oldSize = m_Buf.size();
				int color = 0xFFee0000;
				
				std::string msg = fmt::format(fmt, std::forward<ArgsType>(args)...);
				msg = fmt::format("{}{}\n", "[Info] ", msg);
				fmt::print(fg(fmt::color::sky_blue), "{}", msg);

				m_Buf.append(msg.c_str());
				AddOffset(oldSize, color);
			}
		}

		template <class... ArgsType>
		inline static void Error(std::string_view fmt, ArgsType &&...args)
		{
			if (enabled)
			{
				int oldSize = m_Buf.size();
				int color = 0xFF0000FF;
				
				std::string msg = fmt::format(fmt, std::forward<ArgsType>(args)...);
				msg = fmt::format("{}{}\n", "[Error] ", msg);
				fmt::print(fg(fmt::color::red), "{}", msg);
				
				m_Buf.append(msg.c_str());
				AddOffset(oldSize, color);
			}
		}

		template <class... ArgsType>
		inline static void Success(std::string_view fmt, ArgsType &&...args)
		{
			if (enabled)
			{
				int oldSize = m_Buf.size();
				int color = 0xFF00FF00;

				std::string msg = fmt::format(fmt, std::forward<ArgsType>(args)...);
				msg = fmt::format("{}{}\n", "[Success] ", msg);
				fmt::print(fg(fmt::color::green), "{}", msg);
				
				m_Buf.append(msg.c_str());
				AddOffset(oldSize, color);
			}
		}

		template <class... ArgsType>
		inline static void Warning(std::string_view fmt, ArgsType &&...args)
		{
			if (enabled)
			{
				int oldSize = m_Buf.size();
				int color = 0xFF00FFFF;

				std::string msg = fmt::format(fmt, std::forward<ArgsType>(args)...);
				msg = fmt::format("{}{}\n", "[Warning] ", msg);
				fmt::print(fg(fmt::color::yellow), "{}", msg);

				m_Buf.append(msg.c_str());
				AddOffset(oldSize, color);
			}
		}

		template <class... ArgsType>
		inline static void Print(std::string_view fmt, ArgsType &&...args)
		{
			if (enabled)
			{
				int oldSize = m_Buf.size();
				int color = 0xFFFFFFFF;

				std::string msg = fmt::format(fmt, std::forward<ArgsType>(args)...);
				msg = fmt::format("{}{}\n", "[Print] ", msg);
				fmt::print(fg(fmt::color::white), "{}", msg);

				m_Buf.append(msg.c_str());
				AddOffset(oldSize, color);
			}
		}

		static void ClearBuffer();
		static void Draw(std::string_view title, bool *p_open = nullptr, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
		static bool enabled;

	private:
		static void AddOffset(int oldSize, int color);
		inline Logger() { ClearBuffer(); }
		static Logger s_Instance;
		static ImGuiTextBuffer m_Buf;
		static ImGuiTextFilter m_Filter;
		static ImVector<int> m_LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
		static ImVector<int> m_Colors;
		static bool m_AutoScroll;
	};
}