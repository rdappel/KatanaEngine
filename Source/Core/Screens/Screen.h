#pragma once

#include <functional>
#include <memory>

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
		Screen(bool show = true);
		virtual ~Screen() = default;

		std::function<void()> OnShow;
		std::function<void()> OnShown;
		std::function<void()> OnExit;
		std::function<void()> OnRemoved;

		virtual void Initialize() {}
		virtual void LoadContent(ResourceManager& resourceManager) {}
		virtual void UnloadContent() {}
		virtual void HandleInput(const InputState& inputState) {}
		virtual void Update(const GameTime& gameTime) {}
		virtual void Draw(SpriteBatch& spriteBatch) {}
		virtual void Show();
		virtual void Exit();

		bool IsDrawBlocking() const { return m_isDrawBlocking; }
		void SetDrawBlocking(bool blocking) { m_isDrawBlocking = blocking; }

		bool IsUpdateBlocking() const { return m_isUpdateBlocking; }
		void SetUpdateBlocking(bool blocking) { m_isUpdateBlocking = blocking; }

		bool IsInputBlocking() const { return m_isInputBlocking; }
		void SetInputBlocking(bool blocking) { m_isInputBlocking = blocking; }

		ScreenManager* GetScreenManager() const { return m_pScreenManager.lock().get(); }

	protected:
		bool BeginShow();
		bool BeginExit();
		void RemoveFromManager();
		bool IsExiting() const { return m_isExiting; }

	private:
		friend class ScreenManager;

		void SetScreenManager(std::shared_ptr<ScreenManager> manager) { m_pScreenManager = std::move(manager); }
		bool ShouldShowOnAdd() const { return m_showOnAdd; }
		void NotifyRemoved();

		std::weak_ptr<ScreenManager> m_pScreenManager;

		bool m_isDrawBlocking = true;
		bool m_isUpdateBlocking = true;
		bool m_isInputBlocking = true;
		bool m_showOnAdd = true;
		bool m_isShown = false;
		bool m_isExiting = false;
		bool m_isRemoved = false;
	};
}
