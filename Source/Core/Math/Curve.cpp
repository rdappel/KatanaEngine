#include "pch.h"

#include "Curve.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <locale>
#include <sstream>

namespace KatanaEngine
{
	namespace
	{
		const char* const LoopTypeNames[] = { "Constant", "Cycle", "CycleOffset", "Oscillate", "Linear" };
		const char* const ContinuityNames[] = { "Smooth", "Step" };

		bool EqualsIgnoreCase(const std::string& a, const char* b)
		{
			size_t i = 0;
			for (; i < a.size() && b[i] != '\0'; ++i)
			{
				if (std::tolower(static_cast<unsigned char>(a[i])) != std::tolower(static_cast<unsigned char>(b[i]))) return false;
			}
			return i == a.size() && b[i] == '\0';
		}

		std::string Trim(const std::string& text)
		{
			const size_t start = text.find_first_not_of(" \t\r\n");
			if (start == std::string::npos) return std::string();
			return text.substr(start, text.find_last_not_of(" \t\r\n") - start + 1);
		}

		bool ParseFloat(const std::string& text, float& result)
		{
			std::istringstream stream(Trim(text));
			stream.imbue(std::locale::classic());
			float value = 0.0f;
			if (!(stream >> value) || !(stream >> std::ws).eof()) return false;

			result = value;
			return true;
		}

		bool ParseLoopType(const std::string& text, CurveLoopType& result)
		{
			const std::string name = Trim(text);
			for (int i = 0; i < 5; ++i)
			{
				if (!EqualsIgnoreCase(name, LoopTypeNames[i])) continue;
				result = static_cast<CurveLoopType>(i);
				return true;
			}
			return false;
		}

		bool ParseContinuity(const std::string& text, CurveContinuity& result)
		{
			const std::string name = Trim(text);
			for (int i = 0; i < 2; ++i)
			{
				if (!EqualsIgnoreCase(name, ContinuityNames[i])) continue;
				result = static_cast<CurveContinuity>(i);
				return true;
			}
			return false;
		}

		// The part of a document between two offsets, with helpers for the small amount of
		// XML a curve file needs: elements by local name (namespace prefixes ignored), their
		// text, and the Type attribute of <Asset>. Not a general XML parser.
		struct XmlRange
		{
			const std::string* Text = nullptr;
			size_t Begin = 0;
			size_t End = 0;

			std::string GetText() const { return Text->substr(Begin, End - Begin); }

			// Finds the next element with this local name, starting at from. On success,
			// element is the whole tag and content its inside (empty for <Name/>).
			bool FindElement(const char* name, size_t from, XmlRange& content, size_t& elementEnd, std::string* openTag = nullptr) const
			{
				const std::string& text = *Text;
				const size_t nameLength = std::char_traits<char>::length(name);

				for (size_t position = text.find('<', from); position != std::string::npos && position < End; position = text.find('<', position + 1))
				{
					if (position + 1 >= End || text[position + 1] == '/' || text[position + 1] == '?' || text[position + 1] == '!') continue;

					// Local name: after an optional "prefix:".
					size_t nameStart = position + 1;
					size_t nameEnd = nameStart;
					while (nameEnd < End && (std::isalnum(static_cast<unsigned char>(text[nameEnd])) || text[nameEnd] == '_' || text[nameEnd] == ':' || text[nameEnd] == '.' || text[nameEnd] == '-')) ++nameEnd;
					const size_t colon = text.rfind(':', nameEnd - 1);
					if (colon != std::string::npos && colon >= nameStart && colon < nameEnd) nameStart = colon + 1;
					if (nameEnd - nameStart != nameLength || text.compare(nameStart, nameLength, name) != 0) continue;

					const size_t tagEnd = text.find('>', nameEnd);
					if (tagEnd == std::string::npos || tagEnd >= End) return false;
					if (openTag) *openTag = text.substr(position, tagEnd - position + 1);

					if (text[tagEnd - 1] == '/')	// <Name/>
					{
						content = { Text, tagEnd, tagEnd };
						elementEnd = tagEnd + 1;
						return true;
					}

					// The matching close tag (same qualified name). Curve files don't nest
					// an element inside one of the same name.
					const std::string qualifiedName = text.substr(position + 1, nameEnd - position - 1);
					const size_t close = text.find("</" + qualifiedName, tagEnd + 1);
					if (close == std::string::npos || close >= End) return false;

					content = { Text, tagEnd + 1, close };
					elementEnd = text.find('>', close) + 1;
					return true;
				}
				return false;
			}

			bool GetChildText(const char* name, std::string& result) const
			{
				XmlRange content;
				size_t elementEnd = 0;
				if (!FindElement(name, Begin, content, elementEnd)) return false;

				result = content.GetText();
				return true;
			}
		};

		// Removes the XML declaration and comments, so the scanner never sees them.
		std::string StripMarkup(const std::string& xml)
		{
			std::string result;
			result.reserve(xml.size());
			for (size_t position = 0; position < xml.size();)
			{
				const char* close = nullptr;
				if (xml.compare(position, 4, "<!--") == 0) close = "-->";
				else if (xml.compare(position, 2, "<?") == 0) close = "?>";

				if (!close)
				{
					result += xml[position++];
					continue;
				}

				const size_t end = xml.find(close, position);
				if (end == std::string::npos) break;
				position = end + std::char_traits<char>::length(close);
			}
			return result;
		}

		bool ParseExpandedKeys(const XmlRange& keys, std::vector<CurveKey>& result)
		{
			XmlRange keyContent;
			size_t elementEnd = 0;
			for (size_t from = keys.Begin; keys.FindElement("CurveKey", from, keyContent, elementEnd); from = elementEnd)
			{
				CurveKey key;
				std::string text;
				if (!keyContent.GetChildText("Position", text) || !ParseFloat(text, key.Position)) return false;
				if (!keyContent.GetChildText("Value", text) || !ParseFloat(text, key.Value)) return false;
				if (keyContent.GetChildText("TangentIn", text) && !ParseFloat(text, key.TangentIn)) return false;
				if (keyContent.GetChildText("TangentOut", text) && !ParseFloat(text, key.TangentOut)) return false;
				if (keyContent.GetChildText("Continuity", text) && !ParseContinuity(text, key.Continuity)) return false;
				result.push_back(key);
			}
			return true;
		}

		// XNA's compact form: "position value tangentIn tangentOut continuity" per key.
		bool ParseCompactKeys(const std::string& text, std::vector<CurveKey>& result)
		{
			std::istringstream stream(text);
			std::string token;
			std::vector<std::string> tokens;
			while (stream >> token) tokens.push_back(token);
			if (tokens.size() % 5 != 0) return false;

			for (size_t i = 0; i < tokens.size(); i += 5)
			{
				CurveKey key;
				if (!ParseFloat(tokens[i], key.Position) || !ParseFloat(tokens[i + 1], key.Value)
					|| !ParseFloat(tokens[i + 2], key.TangentIn) || !ParseFloat(tokens[i + 3], key.TangentOut)
					|| !ParseContinuity(tokens[i + 4], key.Continuity)) return false;
				result.push_back(key);
			}
			return true;
		}

		float Hermite(float value0, float tangent0, float value1, float tangent1, float t)
		{
			const float t2 = t * t;
			const float t3 = t2 * t;
			return (2.0f * t3 - 3.0f * t2 + 1.0f) * value0
				+ (t3 - 2.0f * t2 + t) * tangent0
				+ (3.0f * t2 - 2.0f * t3) * value1
				+ (t3 - t2) * tangent1;
		}
	}

	float Curve::Evaluate(float position) const
	{
		if (m_keys.empty()) return 0.0f;

		const CurveKey& first = m_keys.front();
		const CurveKey& last = m_keys.back();
		if (m_keys.size() == 1) return first.Value;

		const bool isBefore = position < first.Position;
		const bool isAfter = position > last.Position;
		if (!isBefore && !isAfter) return EvaluateInRange(position);

		const float range = last.Position - first.Position;
		const CurveLoopType loop = isBefore ? PreLoop : PostLoop;
		if (loop == CurveLoopType::CONSTANT || range <= 0.0f) return isBefore ? first.Value : last.Value;

		if (loop == CurveLoopType::LINEAR)
		{
			return isBefore
				? first.Value - first.TangentIn * (first.Position - position)
				: last.Value + last.TangentOut * (position - last.Position);
		}

		const int cycle = GetCycle(position);
		const float offset = static_cast<float>(cycle) * range;
		if (loop == CurveLoopType::CYCLE) return EvaluateInRange(position - offset);
		if (loop == CurveLoopType::CYCLE_OFFSET) return EvaluateInRange(position - offset) + static_cast<float>(cycle) * (last.Value - first.Value);

		// OSCILLATE: even cycles run forwards, odd cycles backwards.
		const bool isForwards = cycle % 2 == 0;
		return EvaluateInRange(isForwards ? position - offset : last.Position - position + first.Position + offset);
	}

	// XNA's GetNumberOfCycle, including its rounding (truncation, after stepping down once
	// for positions before the first key).
	int Curve::GetCycle(float position) const
	{
		float cycle = (position - m_keys.front().Position) / (m_keys.back().Position - m_keys.front().Position);
		if (cycle < 0.0f) cycle -= 1.0f;
		return static_cast<int>(cycle);
	}

	float Curve::EvaluateInRange(float position) const
	{
		// The segment [previous, next] holding position.
		const auto upper = std::upper_bound(m_keys.begin(), m_keys.end(), position,
			[](float value, const CurveKey& key) { return value < key.Position; });
		const size_t nextIndex = std::clamp(static_cast<size_t>(upper - m_keys.begin()), static_cast<size_t>(1), m_keys.size() - 1);

		const CurveKey& previous = m_keys[nextIndex - 1];
		const CurveKey& next = m_keys[nextIndex];
		const float span = next.Position - previous.Position;
		const float t = span > 0.0f ? (position - previous.Position) / span : 1.0f;

		if (previous.Continuity == CurveContinuity::STEP) return t >= 1.0f ? next.Value : previous.Value;
		return Hermite(previous.Value, previous.TangentOut, next.Value, next.TangentIn, t);
	}

	int Curve::AddKey(const CurveKey& key)
	{
		const auto position = std::upper_bound(m_keys.begin(), m_keys.end(), key.Position,
			[](float value, const CurveKey& existing) { return value < existing.Position; });
		return static_cast<int>(m_keys.insert(position, key) - m_keys.begin());
	}

	void Curve::RemoveKey(int index)
	{
		if (index < 0 || index >= GetKeyCount()) return;
		m_keys.erase(m_keys.begin() + index);
	}

	void Curve::SetKey(int index, float value, float tangentIn, float tangentOut, CurveContinuity continuity)
	{
		if (index < 0 || index >= GetKeyCount()) return;

		CurveKey& key = m_keys[index];
		key.Value = value;
		key.TangentIn = tangentIn;
		key.TangentOut = tangentOut;
		key.Continuity = continuity;
	}

	void Curve::ComputeTangent(int index, CurveTangent tangentIn, CurveTangent tangentOut)
	{
		if (index < 0 || index >= GetKeyCount()) return;

		// The neighbours; a key at either end stands in for its missing neighbour.
		CurveKey& key = m_keys[index];
		const CurveKey& previous = index > 0 ? m_keys[index - 1] : key;
		const CurveKey& next = index + 1 < GetKeyCount() ? m_keys[index + 1] : key;

		const float span = next.Position - previous.Position;
		const float change = next.Value - previous.Value;
		const bool hasSpan = std::abs(span) >= 1.0e-38f;

		switch (tangentIn)
		{
			case CurveTangent::FLAT: key.TangentIn = 0.0f; break;
			case CurveTangent::LINEAR: key.TangentIn = key.Value - previous.Value; break;
			case CurveTangent::SMOOTH: key.TangentIn = hasSpan ? change * ((key.Position - previous.Position) / span) : 0.0f; break;
		}

		switch (tangentOut)
		{
			case CurveTangent::FLAT: key.TangentOut = 0.0f; break;
			case CurveTangent::LINEAR: key.TangentOut = next.Value - key.Value; break;
			case CurveTangent::SMOOTH: key.TangentOut = hasSpan ? change * ((next.Position - key.Position) / span) : 0.0f; break;
		}
	}

	void Curve::ComputeTangents(CurveTangent tangentIn, CurveTangent tangentOut)
	{
		for (int index = 0; index < GetKeyCount(); ++index) ComputeTangent(index, tangentIn, tangentOut);
	}

	bool Curve::LoadFromFile(const std::string& path)
	{
		std::ifstream file(path, std::ios::binary);
		if (!file.is_open()) return false;

		std::ostringstream contents;
		contents << file.rdbuf();
		return LoadFromString(contents.str());
	}

	bool Curve::LoadFromString(const std::string& xml)
	{
		const std::string text = StripMarkup(xml);
		const XmlRange document{ &text, 0, text.size() };

		// <Asset Type="...Curve"> (any namespace prefix on the type).
		XmlRange asset;
		size_t elementEnd = 0;
		std::string assetTag;
		bool found = false;
		for (size_t from = 0; document.FindElement("Asset", from, asset, elementEnd, &assetTag); from = elementEnd)
		{
			const size_t type = assetTag.find("Type=");
			if (type == std::string::npos) continue;

			const char quote = assetTag[type + 5];
			const size_t valueEnd = assetTag.find(quote, type + 6);
			const std::string typeName = assetTag.substr(type + 6, valueEnd - type - 6);
			const size_t separator = typeName.find_last_of(":.");
			if ((separator == std::string::npos ? typeName : typeName.substr(separator + 1)) == "Curve")
			{
				found = true;
				break;
			}
		}
		if (!found) return false;

		CurveLoopType preLoop = CurveLoopType::CONSTANT;
		CurveLoopType postLoop = CurveLoopType::CONSTANT;
		std::string loopText;
		if (asset.GetChildText("PreLoop", loopText) && !ParseLoopType(loopText, preLoop)) return false;
		if (asset.GetChildText("PostLoop", loopText) && !ParseLoopType(loopText, postLoop)) return false;

		XmlRange keysContent;
		std::vector<CurveKey> keys;
		if (asset.FindElement("Keys", asset.Begin, keysContent, elementEnd))
		{
			const bool isExpanded = text.find('<', keysContent.Begin) < keysContent.End;
			const bool parsed = isExpanded ? ParseExpandedKeys(keysContent, keys) : ParseCompactKeys(keysContent.GetText(), keys);
			if (!parsed) return false;
		}

		// Files are written in order, but sort anyway so the curve's invariant holds.
		std::stable_sort(keys.begin(), keys.end(), [](const CurveKey& a, const CurveKey& b) { return a.Position < b.Position; });

		PreLoop = preLoop;
		PostLoop = postLoop;
		m_keys = std::move(keys);
		return true;
	}

	bool Curve::SaveToFile(const std::string& path) const
	{
		std::ofstream file(path, std::ios::binary);
		if (!file.is_open()) return false;

		file << SaveToString();
		return static_cast<bool>(file);
	}

	std::string Curve::SaveToString() const
	{
		std::ostringstream xml;
		xml.imbue(std::locale::classic());
		xml << std::setprecision(9);

		xml << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\r\n"
			<< "<XnaContent xmlns:Framework=\"Microsoft.Xna.Framework\">\r\n"
			<< "  <Asset Type=\"Framework:Curve\">\r\n"
			<< "    <PreLoop>" << LoopTypeNames[static_cast<int>(PreLoop)] << "</PreLoop>\r\n"
			<< "    <PostLoop>" << LoopTypeNames[static_cast<int>(PostLoop)] << "</PostLoop>\r\n"
			<< "    <Keys>\r\n";

		for (const CurveKey& key : m_keys)
		{
			xml << "      <CurveKey>\r\n"
				<< "        <Position>" << key.Position << "</Position>\r\n"
				<< "        <Value>" << key.Value << "</Value>\r\n"
				<< "        <TangentIn>" << key.TangentIn << "</TangentIn>\r\n"
				<< "        <TangentOut>" << key.TangentOut << "</TangentOut>\r\n"
				<< "        <Continuity>" << ContinuityNames[static_cast<int>(key.Continuity)] << "</Continuity>\r\n"
				<< "      </CurveKey>\r\n";
		}

		xml << "    </Keys>\r\n"
			<< "  </Asset>\r\n"
			<< "</XnaContent>\r\n";
		return xml.str();
	}
}
