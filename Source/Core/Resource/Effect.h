#pragma once

#include "Core/Graphics/Color.h"
#include "Core/Math/Vector2.h"

#include <allegro5/allegro.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

namespace KatanaEngine
{
	class Texture;

	class Effect
	{
	public:
		Effect() = default;
		~Effect() = default;

		Effect(Effect&&) noexcept = default;
		Effect& operator=(Effect&&) noexcept = default;

		Effect(const Effect&) = delete;
		Effect& operator=(const Effect&) = delete;

		bool LoadFromFile(const std::string& path);
		bool IsValid() const;

		void Apply() const;
		void ClearParameters();

		void SetFloat(const std::string& name, float value);
		void SetInt(const std::string& name, int value);
		void SetBool(const std::string& name, bool value);
		void SetVector2(const std::string& name, const Vector2& value);
		void SetColor(const std::string& name, const Color& value);
		void SetMatrix(const std::string& name, const ALLEGRO_TRANSFORM& value);
		void SetTexture(const std::string& name, const std::shared_ptr<Texture>& texture, int unit = 0);

	private:
		struct ShaderDeleter
		{
			void operator()(ALLEGRO_SHADER* shader) const;
		};

		struct TextureBinding
		{
			std::shared_ptr<Texture> texture;
			int unit = 0;
		};

		using ParameterValue = std::variant<int, float, bool, Vector2, Color, ALLEGRO_TRANSFORM, TextureBinding>;

		static std::string Trim(const std::string& value);
		bool LoadSourceSections(const std::string& path, std::string& vertexSource, std::string& pixelSource) const;
		void StoreParameter(const std::string& name, const ParameterValue& value);

		std::unique_ptr<ALLEGRO_SHADER, ShaderDeleter> m_shader;
		std::unordered_map<std::string, ParameterValue> m_parameters;
	};
}
