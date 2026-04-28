#include "pch.h"

#include "Effect.h"

#include "Core/Resource/Texture.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace KatanaEngine
{
    void Effect::ShaderDeleter::operator()(ALLEGRO_SHADER* shader) const
    {
        if (shader)
        {
            al_destroy_shader(shader);
        }
    }

    std::string Effect::Trim(const std::string& value)
    {
        const std::string whitespace = " \t\r\n";
        const size_t start = value.find_first_not_of(whitespace);
        if (start == std::string::npos)
        {
            return {};
        }

        const size_t end = value.find_last_not_of(whitespace);
        return value.substr(start, end - start + 1);
    }

    bool Effect::LoadSourceSections(const std::string& path, std::string& vertexSource, std::string& pixelSource) const
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            std::cerr << "Effect: failed to open file: " << path << "\n";
            return false;
        }

        enum class Section
        {
            None,
            Vertex,
            Pixel
        };

        Section section = Section::None;
        std::ostringstream vertexStream;
        std::ostringstream pixelStream;
        std::string line;

        while (std::getline(file, line))
        {
            const std::string trimmed = Trim(line);
            if (trimmed == "[VertexShader]")
            {
                section = Section::Vertex;
                continue;
            }

            if (trimmed == "[/VertexShader]")
            {
                section = Section::None;
                continue;
            }

            if (trimmed == "[PixelShader]")
            {
                section = Section::Pixel;
                continue;
            }

            if (trimmed == "[/PixelShader]")
            {
                section = Section::None;
                continue;
            }

            switch (section)
            {
            case Section::Vertex:
                vertexStream << line << '\n';
                break;
            case Section::Pixel:
                pixelStream << line << '\n';
                break;
            case Section::None:
            default:
                break;
            }
        }

        vertexSource = vertexStream.str();
        pixelSource = pixelStream.str();
        return true;
    }

    bool Effect::LoadFromFile(const std::string& path)
    {
        ClearParameters();
        m_shader.reset();

        std::string vertexSource;
        std::string pixelSource;
        if (!LoadSourceSections(path, vertexSource, pixelSource))
        {
            return false;
        }

        if (vertexSource.empty())
        {
            const char* defaultVertex = al_get_default_shader_source(ALLEGRO_SHADER_AUTO, ALLEGRO_VERTEX_SHADER);
            if (!defaultVertex)
            {
                std::cerr << "Effect: no default vertex shader available for: " << path << "\n";
                return false;
            }

            vertexSource = defaultVertex;
        }

        if (pixelSource.empty())
        {
            const char* defaultPixel = al_get_default_shader_source(ALLEGRO_SHADER_AUTO, ALLEGRO_PIXEL_SHADER);
            if (!defaultPixel)
            {
                std::cerr << "Effect: no default pixel shader available for: " << path << "\n";
                return false;
            }

            pixelSource = defaultPixel;
        }

        m_shader.reset(al_create_shader(ALLEGRO_SHADER_AUTO));
        if (!m_shader)
        {
            std::cerr << "Effect: failed to create shader for: " << path << "\n";
            return false;
        }

        if (!al_attach_shader_source(m_shader.get(), ALLEGRO_VERTEX_SHADER, vertexSource.c_str()))
        {
            std::cerr << "Effect: vertex shader compile failed for: " << path << "\n";
            std::cerr << al_get_shader_log(m_shader.get()) << "\n";
            m_shader.reset();
            return false;
        }

        if (!al_attach_shader_source(m_shader.get(), ALLEGRO_PIXEL_SHADER, pixelSource.c_str()))
        {
            std::cerr << "Effect: pixel shader compile failed for: " << path << "\n";
            std::cerr << al_get_shader_log(m_shader.get()) << "\n";
            m_shader.reset();
            return false;
        }

        if (!al_build_shader(m_shader.get()))
        {
            std::cerr << "Effect: shader link failed for: " << path << "\n";
            std::cerr << al_get_shader_log(m_shader.get()) << "\n";
            m_shader.reset();
            return false;
        }

        return true;
    }

    bool Effect::IsValid() const
    {
        return static_cast<bool>(m_shader);
    }

    void Effect::Apply() const
    {
        if (!m_shader)
        {
            return;
        }

        al_use_shader(m_shader.get());

        for (const auto& [name, value] : m_parameters)
        {
            std::visit(
                [&](const auto& parameter)
                {
                    using T = std::decay_t<decltype(parameter)>;

                    if constexpr (std::is_same_v<T, int>)
                    {
                        al_set_shader_int(name.c_str(), parameter);
                    }
                    else if constexpr (std::is_same_v<T, float>)
                    {
                        al_set_shader_float(name.c_str(), parameter);
                    }
                    else if constexpr (std::is_same_v<T, bool>)
                    {
                        al_set_shader_bool(name.c_str(), parameter);
                    }
                    else if constexpr (std::is_same_v<T, Vector2>)
                    {
                        const float values[2] = { parameter.X, parameter.Y };
                        al_set_shader_float_vector(name.c_str(), 2, values, 1);
                    }
                    else if constexpr (std::is_same_v<T, Color>)
                    {
                        const float values[4] = { parameter.Red, parameter.Green, parameter.Blue, parameter.Alpha };
                        al_set_shader_float_vector(name.c_str(), 4, values, 1);
                    }
                    else if constexpr (std::is_same_v<T, ALLEGRO_TRANSFORM>)
                    {
                        al_set_shader_matrix(name.c_str(), &parameter);
                    }
                    else if constexpr (std::is_same_v<T, TextureBinding>)
                    {
                        if (parameter.texture && parameter.texture->IsValid())
                        {
                            al_set_shader_sampler(name.c_str(), parameter.texture->GetBitmap(), parameter.unit);
                        }
                    }
                },
                value);
        }
    }

    void Effect::ClearParameters()
    {
        m_parameters.clear();
    }

    void Effect::StoreParameter(const std::string& name, const ParameterValue& value)
    {
        m_parameters[name] = value;
    }

    void Effect::SetFloat(const std::string& name, float value)
    {
        StoreParameter(name, value);
    }

    void Effect::SetInt(const std::string& name, int value)
    {
        StoreParameter(name, value);
    }

    void Effect::SetBool(const std::string& name, bool value)
    {
        StoreParameter(name, value);
    }

    void Effect::SetVector2(const std::string& name, const Vector2& value)
    {
        StoreParameter(name, value);
    }

    void Effect::SetColor(const std::string& name, const Color& value)
    {
        StoreParameter(name, value);
    }

    void Effect::SetMatrix(const std::string& name, const ALLEGRO_TRANSFORM& value)
    {
        StoreParameter(name, value);
    }

    void Effect::SetTexture(const std::string& name, const std::shared_ptr<Texture>& texture, int unit)
    {
        StoreParameter(name, TextureBinding{ texture, unit });
    }
}
