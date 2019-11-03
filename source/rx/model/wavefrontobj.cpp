// wavefrontobj.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include <vector>
#include <string_view>

#include "rx.h"
#include "rx/model.h"

namespace rx
{
	Model loadModelFromWavefrontOBJ(AssetLoader::Asset* ass, double scale)
	{
		// split into lines
		Model model;

		std::vector<const char*> lines;

		// copy it over, so we can stomp memory!
		char* copy = new char[ass->length + 1];
		memcpy(copy, ass->raw, ass->length);

		{
			char* view = copy;

			while(true)
			{
				size_t ln = 0;
				while(view[ln] && view[ln] != '\n')
					ln++;

				if(ln > 0)
				{
					view[ln] = 0;
					lines.push_back(view);

					// +1 here because we want to remove the \n from the stream.
					view += (ln + 1);
				}

				if(!view[ln])
					break;
			}
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

		std::vector<lx::vec3> vertices; vertices.reserve(lines.size() / 3);
		std::vector<lx::vec3> normals;  normals.reserve(lines.size() / 3);
		std::vector<lx::vec2> uvs;      uvs.reserve(lines.size() / 3);

		size_t ln = 1;
		for(auto line : lines)
		{
			// fprintf(stderr, "line %zu/%zu\n", ln, lines.size());
			if(line[0] == '#' || line[0] == '\n' || line[0] == '\r')
			{
				ln++;
				continue;
			}
			else if(strncmp(line, "v ", 2) == 0)
			{
				// vertex
				double x = 0;
				double y = 0;
				double z = 0;
				int res = sscanf(line, "v %lf %lf %lf", &x, &y, &z);
				if(res != 3)
					ERROR("Malformed vertex on line %zu", ln);

				// LOG("vertex: %d/%d", ln, lines.size());
				vertices.emplace_back(x, y, z);
			}
			else if(strncmp(line, "vt ", 3) == 0)
			{
				// vertex
				double u = 0;
				double v = 0;

				int res = sscanf(line, "vt %lf %lf", &u, &v);
				if(res != 2)
					ERROR("Malformed UV coordinates on line %zu", ln);

				uvs.emplace_back(u, v);
			}
			else if(strncmp(line, "vn ", 3) == 0)
			{
				// vertex
				double x = 0;
				double y = 0;
				double z = 0;

				int res = sscanf(line, "vn %lf %lf %lf", &x, &y, &z);
				if(res != 3)
					ERROR("Malformed normal on line %zu", ln);

				normals.push_back(lx::vec3(x, y, z).normalised());
			}
			else if(strncmp(line, "f ", 2) == 0)
			{
				// todo: handle the optional cases:
				// f x y z
				// f x/x y/y z/z

				// for now, we only accept either: x/x/x or x//x

				line += 2;
				IndexedFace iface;

				bool failed = false;
				while(line && *line)
				{
					double v = 0; double t = 0; double n = 0;

					// because uv coords may be omitted (the second one -- to give %f//%f), we scan separately.
					if(int didread = 0, res = sscanf(line, "%lf/%n", &v, &didread); res > 0)
					{
						line += didread;
						iface.vertexIndices.push_back(v);

						if(res = sscanf(line, "%lf%n", &t, &didread); res > 0)
						{
							line += didread;
							iface.uvIndices.push_back(t);
						}
						else
						{
							iface.uvIndices.push_back(t);
							res = sscanf(line, "/%lf%n", &n, &didread);

							if(res > 0)
							{
								iface.normalIndices.push_back(n);
								line += didread;
							}
							else
							{
								failed = true;
								break;
							}
						}
					}
					else
					{
						failed = true;
						break;
					}
				}

				if(failed)
				{
					WARN("Malformed OBJ file; unexpected line<%zu>: %s", line, ln);
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
			else if(strncmp(line, "mtllib", 6) == 0 || strncmp(line, "usemtl", 6) == 0)
			{
				// skip
				LOG("Ignoring materials in OBJ file");
			}
			else if(strncmp(line, "o ", 2) == 0)
			{
				// each 'group' would represent a mesh, I guess.
				// not sure if OBJ files *need* a group...

				ObjectGroup og;
				og.name = std::string(line + 2);
				objs.push_back(og);
			}
			else if(strncmp(line, "g ", 2) == 0 || strncmp(line, "s ", 2) == 0)
			{
				// ignore groups and shading
				LOG("Ignoring polygon groups and shading in OBJ file");
			}
			else
			{
				WARN("Malformed OBJ file; unexpected line<%zu>: %s", line, ln);
			}

			ln++;
		}

		size_t faceCount = 0;
		for(const auto& obj : objs)
		{
			Mesh mesh;
			mesh.name = obj.name;

			faceCount += obj.ifaces.size();

			for(const auto& iface : obj.ifaces)
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
