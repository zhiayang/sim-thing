// wavefrontobj.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <vector>
#include <stx/string_view.hpp>

#include <glm/glm.hpp>

#include "renderer/rx.h"
#include "renderer/model.h"

namespace Rx
{
	Model loadModelFromWavefrontOBJ(AssetLoader::Asset* ass, double scale)
	{
		// split into lines
		Model model;

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

		struct IndexedFace
		{
			std::vector<int64_t> vertexIndices;
			std::vector<int64_t> normalIndices;
			std::vector<int64_t> uvIndices;
		};

		struct ObjectGroup
		{
			std::string name;
			std::vector<IndexedFace> ifaces;
		};

		std::vector<ObjectGroup> objs;

		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> uvs;

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

				line.remove_prefix(2);
				IndexedFace iface;

				while(true)
				{
					double v = 0; double t = 0; double n = 0;

					int didread = 0;
					int res = sscanf(line.to_string().c_str(), "%lf/%lf/%lf%n",
						&v, &t, &n, &didread);

					if(res > 0)
					{
						iface.vertexIndices.push_back(v);
						iface.normalIndices.push_back(n);
						iface.uvIndices.push_back(t);

						line.remove_prefix(didread);
						// fprintf(stderr, "face vert %f/%f/%f\n", v, n, t);
					}
					else
					{
						break;
					}
				}

				// fprintf(stderr, "face with %zu verts\n\n", iface.vertexIndices.size());
				if(objs.empty())
				{
					LOG("No object group specified for face, assuming default");
					objs.push_back(ObjectGroup());
				}

				// LOG("face");
				objs.back().ifaces.push_back(iface);
			}
			else if(line.find("mtllib") == 0 || line.find("usemtl") == 0)
			{
				// skip
				LOG("Ignoring materials in OBJ file");
			}
			else if(line.find("o") == 0)
			{
				// each 'group' would represent a mesh, I guess.
				// not sure if OBJ files *need* a group...

				ObjectGroup og;
				og.name = line.substr(2).to_string();
				objs.push_back(og);
			}
			else if(line.find("g") == 0 || line.find("s") == 0)
			{
				// ignore groups and shading
				LOG("Ingoring polygon groups and shading in OBJ file");
			}
			else
			{
				WARN("Malformed OBJ file; unexpected token <%d/%s> at beginning of line <%zu>",
					line[0], line.substr(0, line.find(' ')).to_string().c_str(), ln);
			}

			ln++;
		}

		size_t faceCount = 0;
		for(auto obj : objs)
		{
			Mesh mesh;
			mesh.name = obj.name;

			faceCount += obj.ifaces.size();

			for(auto iface : obj.ifaces)
			{
				Face face;
				for(auto vi : iface.vertexIndices)
					face.vertices.push_back(vertices[vi - 1] * (float) scale);

				for(auto ni : iface.normalIndices)
					face.normals.push_back(normals[ni - 1]);

				for(auto ti : iface.uvIndices)
					face.uvs.push_back(uvs[ti - 1]);


				if(face.vertices.size() != 3 && face.vertices.size() != 4)
				{
					ERROR("Unsupported OBJ file with '%zu' vertices per face; only triangles (3) or quads (4) are supported",
						face.vertices.size());
				}

				if(face.vertices.size() == 4)
					face = triangulateQuadFace(face);

				face.faceNormal = calculateNormalForFace(face);

				mesh.faces.push_back(face);
			}

			model.addMesh(mesh, Material());
		}

		LOG("Loaded model with %zu faces", faceCount);

		return model;
	}
}
