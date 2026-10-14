#pragma once

#include "Core/Resource/TextResource.h"

#include <string>
#include <vector>

namespace KatanaEngine
{
	// What a Curve returns outside the range of its keys.
	enum class CurveLoopType
	{
		CONSTANT,		// the first / last key's value
		CYCLE,			// repeat the curve
		CYCLE_OFFSET,	// repeat, adding the change across the curve each cycle
		OSCILLATE,		// repeat, alternating forwards and backwards
		LINEAR			// extend along the first / last key's tangent
	};

	// How Curve::ComputeTangent sets a key's tangent.
	enum class CurveTangent
	{
		FLAT,			// zero slope
		LINEAR,			// towards the neighbouring key
		SMOOTH			// through both neighbours, scaled for uneven key spacing
	};

	// How the curve leaves a key.
	enum class CurveContinuity
	{
		SMOOTH,			// interpolate to the next key
		STEP			// hold this key's value until the next key
	};

	// A point on a Curve. Tangents are measured per segment (the change over the whole
	// segment between two keys), not per unit of position, as in XNA.
	struct CurveKey
	{
		float Position = 0.0f;
		float Value = 0.0f;
		float TangentIn = 0.0f;
		float TangentOut = 0.0f;
		CurveContinuity Continuity = CurveContinuity::SMOOTH;
	};

	// A 1D keyframed curve, compatible with XNA's Microsoft.Xna.Framework.Curve: the same
	// keys, tangents, loop types and evaluation, so a curve gives the same values in XNA,
	// MonoGame and KatanaEngine.
	//
	// Files are XNA content XML (one curve per file), as written by XNA's IntermediateSerializer,
	// the XNA Curve Editor sample and MonoGame:
	//
	//     <XnaContent xmlns:Framework="Microsoft.Xna.Framework">
	//       <Asset Type="Framework:Curve">
	//         <PreLoop>Constant</PreLoop>
	//         <PostLoop>Constant</PostLoop>
	//         <Keys>
	//           <CurveKey><Position>0</Position><Value>1</Value><TangentIn>0</TangentIn><TangentOut>0</TangentOut><Continuity>Smooth</Continuity></CurveKey>
	//         </Keys>
	//       </Asset>
	//     </XnaContent>
	//
	// Keys may also use XNA's compact form: "position value tangentIn tangentOut continuity"
	// for each key, separated by spaces. Load one with ResourceManager::Load<Curve>.
	class Curve : public TextResource
	{
	public:
		Curve() = default;
		~Curve() override = default;

		Curve(const Curve&) = default;
		Curve& operator=(const Curve&) = default;

		CurveLoopType PreLoop = CurveLoopType::CONSTANT;	// before the first key
		CurveLoopType PostLoop = CurveLoopType::CONSTANT;	// after the last key

		// The value at position. With no keys it is 0; with one key, that key's value.
		float Evaluate(float position) const;

		// Inserts a key in position order and returns its index. A key at the same position
		// as existing keys goes after them.
		int AddKey(const CurveKey& key);
		void RemoveKey(int index);
		void Clear() { m_keys.clear(); }

		int GetKeyCount() const { return static_cast<int>(m_keys.size()); }
		const CurveKey& GetKey(int index) const { return m_keys[index]; }

		// Changes everything except the position (to move a key, remove it and add it again,
		// which keeps the keys in order).
		void SetKey(int index, float value, float tangentIn, float tangentOut, CurveContinuity continuity);

		// True with fewer than two keys: the curve has the same value everywhere.
		bool IsConstant() const { return m_keys.size() <= 1; }

		void ComputeTangent(int index, CurveTangent tangent) { ComputeTangent(index, tangent, tangent); }
		void ComputeTangent(int index, CurveTangent tangentIn, CurveTangent tangentOut);
		void ComputeTangents(CurveTangent tangent) { ComputeTangents(tangent, tangent); }
		void ComputeTangents(CurveTangent tangentIn, CurveTangent tangentOut);

		// Reads an XNA curve XML file. On failure returns false and leaves the curve unchanged.
		bool LoadFromFile(const std::string& path) override;

		// Parses XNA curve XML text (see LoadFromFile).
		bool LoadFromString(const std::string& xml);

		// Writes an XNA curve XML file (keys in the expanded form, which XNA and MonoGame read).
		bool SaveToFile(const std::string& path) const;
		std::string SaveToString() const;

	private:
		float EvaluateInRange(float position) const;
		int GetCycle(float position) const;

		std::vector<CurveKey> m_keys;	// always sorted by Position
	};
}
