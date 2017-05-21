// model.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "model.h"
#include "utilities.h"

#include <glm/glm.hpp>

#include <sstream>

#include <stx/string_view.hpp>

using namespace AssetLoader;

// fuck this shit.
namespace glm
{
	template <typename T>
	T normalise(const T& thing)
	{
		return glm::normalize(thing);
	}
}

namespace Rx
{
	static void loadWavefrontOBJIntoModel(Asset* asset, Model* model);


	Model* loadModelFromAsset(Asset* asset)
	{
		Model* model = new Model();

		switch(asset->type)
		{
			case AssetType::ModelOBJ: {
				loadWavefrontOBJIntoModel(asset, model);
			}
			break;

			default:
				ERROR("Invalid asset to load model from (type %d)", asset->type);

		}

		return model;
	}




	static void loadWavefrontOBJIntoModel(Asset* ass, Model* mdl)
	{
		// split into lines
		std::vector<stx::string_view> lines;
		{
			auto view = stx::string_view((char*) ass->raw, ass->length);

			while(true)
			{
				size_t ln = view.find("\n");

				if(ln != stx::string_view::npos)
				{
					// fuck windows line endings
					if(view.length() > 1 && view.data()[ln - 1] == '\r')
					{
						// don't +1 because we don't want the \n in the line itself
						lines.push_back(stx::string_view(view.data(), ln - 1));
					}
					else
					{
						// don't +1 because we don't want the \n in the line itself
						lines.push_back(stx::string_view(view.data(), ln));
					}

					// +1 here because we want to remove the \n from the stream.
					view.remove_prefix(ln + 1);
				}
				else
				{
					break;
				}
			}
		}

		for(auto& line : lines)
		{
			while(line.find(' ') == 0)
				line.remove_prefix(1);
		}

		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;

		std::vector<int64_t> vertexIndices;
		std::vector<int64_t> normalIndices;
		std::vector<int64_t> uvIndices;

		size_t ln = 1;
		for(auto line : lines)
		{
			if(line.empty() || line[0] == '#' || line[0] == '\n' || line[0] == '\r')
			{
				ln++;
				continue;
			}
			else if(line.find("v ") == 0)
			{
				// vertex
				double x = 0;
				double y = 0;
				double z = 0;
				int res = sscanf(line.to_string().c_str(), "v %lf %lf %lf", &x, &y, &z);
				if(res != 3)
					ERROR("Malformed vertex on line %zu", ln);

				vertices.push_back(glm::vec3(x, y, z));
			}
			else if(line.find("vt ") == 0)
			{
				// vertex
				double u = 0;
				double v = 0;

				int res = sscanf(line.to_string().c_str(), "vt %lf %lf", &u, &v);
				if(res != 2)
					ERROR("Malformed UV coordinates on line %zu", ln);

				uvs.push_back(glm::vec2(u, v));
			}
			else if(line.find("vn ") == 0)
			{
				// vertex
				double x = 0;
				double y = 0;
				double z = 0;

				int res = sscanf(line.to_string().c_str(), "vn %lf %lf %lf", &x, &y, &z);
				if(res != 3)
					ERROR("Malformed normal on line %zu", ln);

				normals.push_back(glm::normalise(glm::vec3(x, y, z)));
			}
			else if(line.find("f") == 0)
			{
				// todo: handle the optional cases:
				// f x y z
				// f x/x y/y z/z
				// f x//x y//y z//z

				// for now, only accept the full form: f x/x/x y/y/y z/z/z

				double vx = 0; double tx = 0; double nx = 0;
				double vy = 0; double ty = 0; double ny = 0;
				double vz = 0; double tz = 0; double nz = 0;

				int res = sscanf(line.to_string().c_str(), "f %lf/%lf/%lf %lf/%lf/%lf %lf/%lf/%lf",
					&vx, &tx, &nx, &vy, &ty, &ny, &vz, &tz, &nz);

				vertexIndices.push_back(vx);
				vertexIndices.push_back(vy);
				vertexIndices.push_back(vz);

				normalIndices.push_back(nx);
				normalIndices.push_back(ny);
				normalIndices.push_back(nz);

				uvIndices.push_back(tx);
				uvIndices.push_back(ty);
				uvIndices.push_back(tz);

				if(res != 9)
					ERROR("Malformed face on line %zu", ln);
			}
			else if(line.find("mtllib") == 0 || line.find("usemtl") == 0)
			{
				// skip
				LOG("Ignoring materials in OBJ file");
			}
			else if(line.find("o") == 0 || line.find("g") == 0 || line.find("s") == 0)
			{
				// ignore groups and shading
				LOG("Ignoring object groups and shading in OBJ file");
			}
			else
			{
				WARN("Malformed OBJ file; unexpected token <%d/%s> at beginning of line <%zu>",
					line[0], line.substr(0, line.find(' ')).to_string().c_str(), ln);
			}

			ln++;
		}

		// get all the vertices
		for(size_t i = 0; i < vertexIndices.size(); i++)
		{
			auto vi = vertexIndices[i];
			auto ni = normalIndices[i];
			auto ti = uvIndices[i];

			// -1 because OBJ indices start from 1
			glm::vec3 vertex = vertices[vi - 1];
			glm::vec3 normal = normals[ni - 1];
			glm::vec2 uv = uvs[ti - 1];

			mdl->vertices.push_back(vertex);
			mdl->normals.push_back(normal);
			mdl->uvs.push_back(uv);
		}
	}
}




















