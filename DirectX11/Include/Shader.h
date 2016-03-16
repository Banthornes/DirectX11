#pragma once

class Shader
{
public:
    Shader(const std::string& filename, const std::string& name);

    std::string GetName() const;

private:
    std::string m_name;
};