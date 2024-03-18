#include "Utils/Logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

namespace Util
{
	ImGuiTextBuffer Logger::m_Buf{};
	ImGuiTextFilter Logger::m_Filter{};
	ImVector<int> Logger::m_LineOffsets{};
	ImVector<int> Logger::m_Colors{};
	bool Logger::enabled = true;
	bool Logger::m_AutoScroll = true;
	Logger Logger::s_Instance{};

	void Logger::AddOffset(int oldSize, int color)
	{
		for (int newSize = m_Buf.size(); oldSize < newSize; oldSize++)
		{
			auto value = m_Buf[oldSize];
			if (value == '\n')
			{
				m_LineOffsets.push_back(oldSize + 1);
				m_Colors.push_back(color);
			}
		}
	}

	void Logger::ClearBuffer()
	{
		m_LineOffsets.clear();
		m_Buf.clear();
		m_Colors.clear();
		Info("Initializing Logger");
	}

	void Logger::Draw(std::string_view title, bool *p_open, ImGuiWindowFlags flags)
	{
		if (!ImGui::Begin(title.data(), p_open, flags))
		{
			ImGui::End();
			return;
		}
		if (ImGui::BeginPopup("Options"))
		{
			ImGui::Checkbox("Auto-scroll", &m_AutoScroll);
			ImGui::EndPopup();
		}
		if (ImGui::Button("Options"))
			ImGui::OpenPopup("Options");
		ImGui::SameLine();
		if (ImGui::Button("Clear"))
			ClearBuffer();
		ImGui::SameLine();
		if (ImGui::Button("Copy"))
			ImGui::LogToClipboard();
		ImGui::Separator();
		ImGui::TextUnformatted("Filter");
		ImGui::SameLine();
		m_Filter.Draw("###Filter", -100.0f);
		ImGui::Separator();
		ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		const char *buf = m_Buf.begin();
		const char *buf_end = m_Buf.end();
		if (m_Filter.IsActive())
		{
			for (int line_no = 0; line_no < m_LineOffsets.Size; line_no++)
			{
				const char *line_start = buf + m_LineOffsets[line_no];
				const char *line_end = (line_no + 1 < m_LineOffsets.Size) ? (buf + m_LineOffsets[line_no + 1] - 1) : buf_end;
				if (m_Filter.PassFilter(line_start, line_end))
				{
					if (line_no + 1 >= m_Colors.Size)
						ImGui::PushStyleColor(ImGuiCol_Text, ImU32(m_Colors[m_Colors.Size - 1]));
					else
						ImGui::PushStyleColor(ImGuiCol_Text, ImU32(m_Colors[line_no + 1]));
					ImGui::TextUnformatted(line_start, line_end);
					ImGui::PopStyleColor();
				}
			}
		}
		else
		{
			ImGuiListClipper clipper;
			clipper.Begin(m_LineOffsets.Size);
			while (clipper.Step())
			{
				for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
				{
					int offset = (false) ? 0 : m_LineOffsets[line_no];
					const char *line_start = buf + offset;
					const char *line_end = (line_no + 1 < m_LineOffsets.Size) ? (buf + m_LineOffsets[line_no + 1] - 1) : buf_end;
					if (line_no + 1 >= m_Colors.Size)
						ImGui::PushStyleColor(ImGuiCol_Text, ImU32(m_Colors[m_Colors.Size - 1]));
					else
						ImGui::PushStyleColor(ImGuiCol_Text, ImU32(m_Colors[line_no + 1]));
					ImGui::TextUnformatted(line_start, line_end);
					ImGui::PopStyleColor();
				}
			}
			clipper.End();
		}
		ImGui::PopStyleVar();

		if (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);

		ImGui::EndChild();
		ImGui::End();
	}
}