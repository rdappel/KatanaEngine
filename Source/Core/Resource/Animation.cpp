#include "pch.h"

#include "Animation.h"

namespace KatanaEngine
{
	Animation::Animation()
		: m_frameWidth(0)
		, m_frameHeight(0)
		, m_frameDuration(0.1f)
		, m_loop(true)
		, m_framesPerRow(1)
		, m_currentFrame(0)
		, m_elapsedTime(0.0f)
		, m_finished(false)
	{
	}

	void Animation::Update(float deltaTime)
	{
		if (m_frameIndices.empty() || m_finished) return;

		m_elapsedTime += deltaTime;

		while (m_elapsedTime >= m_frameDuration)
		{
			m_elapsedTime -= m_frameDuration;
			m_currentFrame++;

			if (m_currentFrame >= static_cast<int>(m_frameIndices.size()))
			{
				if (m_loop)
				{
					m_currentFrame = 0;
				}
				else
				{
					m_currentFrame = static_cast<int>(m_frameIndices.size()) - 1;
					m_finished = true;
				}
			}
		}
	}

	void Animation::Reset()
	{
		m_currentFrame = 0;
		m_elapsedTime = 0.0f;
		m_finished = false;
	}

	Region Animation::GetSourceRect() const
	{
		if (m_frameIndices.empty()) return Region(0, 0, m_frameWidth, m_frameHeight);

		int frameIndex = m_frameIndices[m_currentFrame];
		int col = frameIndex % m_framesPerRow;
		int row = frameIndex / m_framesPerRow;

		return Region(col * m_frameWidth, row * m_frameHeight, m_frameWidth, m_frameHeight);
	}

	Animation Animation::Clone() const
	{
		Animation clone(*this);
		clone.Reset();
		return clone;
	}
}
