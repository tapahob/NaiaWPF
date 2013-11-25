#pragma once
struct Material
{
	Material()
	{
		Material(Color4(1.0f), Color4(1.0f), Color4(1.0f), Color4(1.0f));
	}

	Material(const Color4 ambient, const Color4 diffuse, const Color4 specular, const Color4 emissive)
	: Ambient(ambient), Diffuse(diffuse), Emissive(emissive), Specular(specular)
	{
	}

	Color4 Ambient;
	Color4 Diffuse;
	Color4 Specular;
	Color4 Emissive;
};