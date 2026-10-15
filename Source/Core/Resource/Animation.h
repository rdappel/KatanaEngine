#pragma once

#include "Core/Math/Region.h"

#include <vector>

namespace KatanaEngine
{
	class AnimationSet;

	class Animation
	{
	public:
		Animation();
		Animation(const Animation& other) = default;
		Animation& operator=(const Animation& other) = default;
		Animation(Animation&& other) = default;
		Animation& operator=(Animation&& other) = default;

		// Definition properties
		int GetFrameWidth() const { return m_frameWidth; }
		int GetFrameHeight() const { return m_frameHeight; }
		float GetFrameDuration() const { return m_frameDuration; }
		bool IsLooping() const { return m_loop; }
		int GetFrameCount() const { return static_cast<int>(m_frameIndices.size()); }

		// Runtime state
		void Update(float deltaTime);
		void Reset();

		Region GetSourceRect() const;
		int GetCurrentFrameIndex() const { return m_currentFrame; }
		bool IsFinished() const { return m_finished; }
		bool IsValid() const { return !m_frameIndices.empty(); }

		// Create an independent copy with reset state
		Animation Clone() const;

	private:
		friend class AnimationSet;

		// Definition (shared when cloned, but copied for simplicity)
		int m_frameWidth = 0;
		int m_frameHeight = 0;
		std::vector<int> m_frameIndices;
		float m_frameDuration = 0.1f;
		bool m_loop = true;
		int m_framesPerRow = 1;

		// Runtime state
		int m_currentFrame = 0;
		float m_elapsedTime = 0.0f;
		bool m_finished = false;
	};
}
