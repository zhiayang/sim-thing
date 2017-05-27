// model.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "model.h"
#include "utilities.h"

#include "earcut.h"

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
	// given 4 vertices, returns 6 vertices which are the triangles of the quad.
	static Model::Face triangulateQuadFace(Model::Face face);

	static void loadWavefrontOBJIntoModel(Asset* asset, Model* model);


	Model* loadModelFromAsset(Asset* asset)
	{
		Model* model = new Model();

		switch(asset->type)
		{
			case AssetType::ModelOBJ: {
				loadWavefrontOBJIntoModel(asset, model);
			} break;

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

		struct IndexedFace
		{
			std::vector<int64_t> vertexIndices;
			std::vector<int64_t> normalIndices;
			std::vector<int64_t> uvIndices;
		};

		std::vector<IndexedFace> ifaces;

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
						fprintf(stderr, "face vert %f/%f/%f\n", v, n, t);
					}
					else
					{
						break;
					}
				}

				fprintf(stderr, "face with %zu verts\n\n", iface.vertexIndices.size());
				ifaces.push_back(iface);
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

		for(auto iface : ifaces)
		{
			using Face = Model::Face;

			Face face;
			for(auto vi : iface.vertexIndices)
				face.vertices.push_back(vertices[vi - 1] / 10000.0f);

			for(auto ni : iface.normalIndices)
				face.normals.push_back(normals[ni - 1]);

			for(auto ti : iface.uvIndices)
				face.uvs.push_back(uvs[ti - 1]);


			if(face.vertices.size() != 3 && face.vertices.size() != 4)
				ERROR("Unsupported OBJ file with '%zu' vertices per face; only triangles (3) or quads (4) are supported", face.vertices.size());

			// mdl->faces.push_back(face);
			if(face.vertices.size() == 4)
				mdl->faces.push_back(triangulateQuadFace(face));

			else
				mdl->faces.push_back(face);
		}
	}


	static Model::Face triangulateQuadFace(Model::Face face)
	{
		std::vector<glm::vec3> verts;
		std::vector<glm::vec3> norms;
		std::vector<glm::vec2> uvs;

		// only supprot quads
		assert(face.vertices.size() == 4 && "only quads supported");
		auto a = face.vertices[0];
		auto b = face.vertices[1];
		auto c = face.vertices[2];
		auto d = face.vertices[3];

		auto an = face.normals[0];
		auto bn = face.normals[1];
		auto cn = face.normals[2];
		auto dn = face.normals[3];

		auto at = face.uvs[0];
		auto bt = face.uvs[1];
		auto ct = face.uvs[2];
		auto dt = face.uvs[3];

		auto ac = glm::distance(a, c);
		auto bd = glm::distance(b, d);

		if(ac < bd)
		{
			verts.push_back(a);		uvs.push_back(at);		norms.push_back(an);
			verts.push_back(b);		uvs.push_back(bt);		norms.push_back(an);
			verts.push_back(c);		uvs.push_back(ct);		norms.push_back(an);

			verts.push_back(a);		uvs.push_back(at);		norms.push_back(an);
			verts.push_back(c);		uvs.push_back(ct);		norms.push_back(an);
			verts.push_back(d);		uvs.push_back(dt);		norms.push_back(an);
		}
		else
		{
			verts.push_back(a);		uvs.push_back(at);		norms.push_back(an);
			verts.push_back(b);		uvs.push_back(bt);		norms.push_back(bn);
			verts.push_back(d);		uvs.push_back(dt);		norms.push_back(dn);

			verts.push_back(d);		uvs.push_back(dt);		norms.push_back(dn);
			verts.push_back(b);		uvs.push_back(bt);		norms.push_back(bn);
			verts.push_back(c);		uvs.push_back(ct);		norms.push_back(cn);
		}

		Model::Face ret;

		ret.vertices = verts;
		ret.normals = norms;
		ret.uvs = uvs;

		return ret;
	}
}




















