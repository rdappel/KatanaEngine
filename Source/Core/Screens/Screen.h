#pragma once

namespace KatanaEngine
{
	struct GameTime;
	class InputState;
	class ResourceManager;
	class ScreenManager;
	class SpriteBatch;

	class Screen
	{
	public:
		Screen();
		virtual ~Screen() = default;

		virtual void Initialize() {}
		virtual void LoadContent(ResourceManager& resourceManager) {}
		virtual void UnloadContent() {}
		virtual void HandleInput(const InputState& inputState) {}
		virtual void Update(const GameTime& gameTime) {}
		virtual void Draw(SpriteBatch& spriteBatch) {}

		bool IsDrawBlocking() const { return m_isDrawBlocking; }
		void SetDrawBlocking(bool blocking) { m_isDrawBlocking = blocking; }

		bool IsUpdateBlocking() const { return m_isUpdateBlocking; }
		void SetUpdateBlocking(bool blocking) { m_isUpdateBlocking = blocking; }

		bool IsInputBlocking() const { return m_isInputBlocking; }
		void SetInputBlocking(bool blocking) { m_isInputBlocking = blocking; }

		ScreenManager* GetScreenManager() const { return m_pScreenManager; }

	private:
		friend class ScreenManager;
		void SetScreenManager(ScreenManager* manager) { m_pScreenManager = manager; }

		ScreenManager* m_pScreenManager = nullptr;
		bool m_isDrawBlocking = true;
		bool m_isUpdateBlocking = true;
		bool m_isInputBlocking = true;
	};
}
