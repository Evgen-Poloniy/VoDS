#pragma once
#include <string>
#include <iostream>
#include <fstream>
//-------------------------------------------------------------------
class Shader
{
public:
	Shader(const std::string& vertPath, const std::string& fragPath);
	~Shader();

private:
	void LoadFile();
};
//-------------------------------------------------------------------