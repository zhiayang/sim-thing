// swizzling.cpp
// Copyright (c) 2014 - 2016, zhiayang@gmail.com
// Licensed under the Apache License Version 2.0.

#include "lx.h"

namespace lx
{

	// vec2
	vec2 vec2::xy() const { return *this; }
	vec2 vec2::yx() const { return vec2(y, x); }

	vec2 vec2::xx() const { return vec2(x); }
	vec2 vec2::yy() const { return vec2(y); }



	// vec3
	vec2 vec3::xy() const { return vec2(x, y); }
	vec2 vec3::yx() const { return vec2(y, x); }

	vec3 vec3::xyz() const { return vec3(x, y, z); }
	vec3 vec3::xzy() const { return vec3(x, z, y); }
	vec3 vec3::yxz() const { return vec3(y, x, z); }
	vec3 vec3::yzx() const { return vec3(y, z, x); }
	vec3 vec3::zxy() const { return vec3(z, x, y); }
	vec3 vec3::zyx() const { return vec3(z, y, x); }

	vec3 vec3::rgb() const { return vec3(r, g, b); }
	vec3 vec3::bgr() const { return vec3(b, g, r); }

	vec2 vec3::xx() const { return vec2(x); }
	vec2 vec3::yy() const { return vec2(y); }
	vec2 vec3::zz() const { return vec2(z); }

	vec3 vec3::xxx() const { return vec3(x); }
	vec3 vec3::yyy() const { return vec3(y); }
	vec3 vec3::zzz() const { return vec3(z); }



	// vec4
	vec2 vec4::xy() const { return vec2(x, y); }
	vec2 vec4::yx() const { return vec2(y, x); }

	vec3 vec4::xyz() const { return vec3(x, y, z); }
	vec3 vec4::xzy() const { return vec3(x, z, y); }
	vec3 vec4::yxz() const { return vec3(y, x, z); }
	vec3 vec4::yzx() const { return vec3(y, z, x); }
	vec3 vec4::zxy() const { return vec3(z, x, y); }
	vec3 vec4::zyx() const { return vec3(z, y, x); }

	vec3 vec4::rgb() const { return vec3(r, g, b); }
	vec3 vec4::bgr() const { return vec3(b, g, r); }

	vec4 vec4::xyzw() const { return vec4(x, y, z, w); }
	vec4 vec4::xywz() const { return vec4(x, y, w, z); }
	vec4 vec4::xzyw() const { return vec4(x, z, y, w); }
	vec4 vec4::xzwy() const { return vec4(x, z, w, y); }
	vec4 vec4::xwyz() const { return vec4(x, w, y, z); }
	vec4 vec4::xwzy() const { return vec4(x, w, z, y); }
	vec4 vec4::yxwz() const { return vec4(y, x, w, z); }
	vec4 vec4::yxzw() const { return vec4(y, x, z, w); }
	vec4 vec4::yzwx() const { return vec4(y, z, w, x); }
	vec4 vec4::yzxw() const { return vec4(y, z, x, w); }
	vec4 vec4::ywzx() const { return vec4(y, w, z, x); }
	vec4 vec4::ywxz() const { return vec4(y, w, x, z); }
	vec4 vec4::zxyw() const { return vec4(z, x, y, w); }
	vec4 vec4::zxwy() const { return vec4(z, x, w, y); }
	vec4 vec4::zyxw() const { return vec4(z, y, x, w); }
	vec4 vec4::zywx() const { return vec4(z, y, w, x); }
	vec4 vec4::zwxy() const { return vec4(z, w, x, y); }
	vec4 vec4::zwyx() const { return vec4(z, w, y, x); }
	vec4 vec4::wxzy() const { return vec4(w, x, z, y); }
	vec4 vec4::wxyz() const { return vec4(w, x, y, z); }
	vec4 vec4::wyzx() const { return vec4(w, y, z, x); }
	vec4 vec4::wyxz() const { return vec4(w, y, x, z); }
	vec4 vec4::wzyx() const { return vec4(w, z, y, x); }
	vec4 vec4::wzxy() const { return vec4(w, z, x, y); }

	vec4 vec4::rgba() const { return vec4(r, g, b, a); }
	vec4 vec4::bgra() const { return vec4(b, g, r, a); }
	vec4 vec4::argb() const { return vec4(a, r, g, b); }
	vec4 vec4::abgr() const { return vec4(a, b, g, r); }

	vec2 vec4::xx() const { return vec2(x); }
	vec2 vec4::yy() const { return vec2(y); }
	vec2 vec4::zz() const { return vec2(z); }
	vec2 vec4::ww() const { return vec2(w); }

	vec3 vec4::xxx() const { return vec3(x); }
	vec3 vec4::yyy() const { return vec3(y); }
	vec3 vec4::zzz() const { return vec3(z); }
	vec3 vec4::www() const { return vec3(w); }

	vec4 vec4::xxxx() const { return vec4(x); }
	vec4 vec4::yyyy() const { return vec4(y); }
	vec4 vec4::zzzz() const { return vec4(z); }
	vec4 vec4::wwww() const { return vec4(w); }






	// fvec2
	fvec2 fvec2::xy() const { return *this; }
	fvec2 fvec2::yx() const { return fvec2(y, x); }

	fvec2 fvec2::xx() const { return fvec2(x); }
	fvec2 fvec2::yy() const { return fvec2(y); }



	// fvec3
	fvec2 fvec3::xy() const { return fvec2(x, y); }
	fvec2 fvec3::yx() const { return fvec2(y, x); }

	fvec3 fvec3::xyz() const { return fvec3(x, y, z); }
	fvec3 fvec3::xzy() const { return fvec3(x, z, y); }
	fvec3 fvec3::yxz() const { return fvec3(y, x, z); }
	fvec3 fvec3::yzx() const { return fvec3(y, z, x); }
	fvec3 fvec3::zxy() const { return fvec3(z, x, y); }
	fvec3 fvec3::zyx() const { return fvec3(z, y, x); }

	fvec3 fvec3::rgb() const { return fvec3(r, g, b); }
	fvec3 fvec3::bgr() const { return fvec3(b, g, r); }

	fvec2 fvec3::xx() const { return fvec2(x); }
	fvec2 fvec3::yy() const { return fvec2(y); }
	fvec2 fvec3::zz() const { return fvec2(z); }

	fvec3 fvec3::xxx() const { return fvec3(x); }
	fvec3 fvec3::yyy() const { return fvec3(y); }
	fvec3 fvec3::zzz() const { return fvec3(z); }



	// fvec4
	fvec2 fvec4::xy() const { return fvec2(x, y); }
	fvec2 fvec4::yx() const { return fvec2(y, x); }

	fvec3 fvec4::xyz() const { return fvec3(x, y, z); }
	fvec3 fvec4::xzy() const { return fvec3(x, z, y); }
	fvec3 fvec4::yxz() const { return fvec3(y, x, z); }
	fvec3 fvec4::yzx() const { return fvec3(y, z, x); }
	fvec3 fvec4::zxy() const { return fvec3(z, x, y); }
	fvec3 fvec4::zyx() const { return fvec3(z, y, x); }

	fvec3 fvec4::rgb() const { return fvec3(r, g, b); }
	fvec3 fvec4::bgr() const { return fvec3(b, g, r); }

	fvec4 fvec4::xyzw() const { return fvec4(x, y, z, w); }
	fvec4 fvec4::xywz() const { return fvec4(x, y, w, z); }
	fvec4 fvec4::xzyw() const { return fvec4(x, z, y, w); }
	fvec4 fvec4::xzwy() const { return fvec4(x, z, w, y); }
	fvec4 fvec4::xwyz() const { return fvec4(x, w, y, z); }
	fvec4 fvec4::xwzy() const { return fvec4(x, w, z, y); }
	fvec4 fvec4::yxwz() const { return fvec4(y, x, w, z); }
	fvec4 fvec4::yxzw() const { return fvec4(y, x, z, w); }
	fvec4 fvec4::yzwx() const { return fvec4(y, z, w, x); }
	fvec4 fvec4::yzxw() const { return fvec4(y, z, x, w); }
	fvec4 fvec4::ywzx() const { return fvec4(y, w, z, x); }
	fvec4 fvec4::ywxz() const { return fvec4(y, w, x, z); }
	fvec4 fvec4::zxyw() const { return fvec4(z, x, y, w); }
	fvec4 fvec4::zxwy() const { return fvec4(z, x, w, y); }
	fvec4 fvec4::zyxw() const { return fvec4(z, y, x, w); }
	fvec4 fvec4::zywx() const { return fvec4(z, y, w, x); }
	fvec4 fvec4::zwxy() const { return fvec4(z, w, x, y); }
	fvec4 fvec4::zwyx() const { return fvec4(z, w, y, x); }
	fvec4 fvec4::wxzy() const { return fvec4(w, x, z, y); }
	fvec4 fvec4::wxyz() const { return fvec4(w, x, y, z); }
	fvec4 fvec4::wyzx() const { return fvec4(w, y, z, x); }
	fvec4 fvec4::wyxz() const { return fvec4(w, y, x, z); }
	fvec4 fvec4::wzyx() const { return fvec4(w, z, y, x); }
	fvec4 fvec4::wzxy() const { return fvec4(w, z, x, y); }

	fvec4 fvec4::rgba() const { return fvec4(r, g, b, a); }
	fvec4 fvec4::bgra() const { return fvec4(b, g, r, a); }
	fvec4 fvec4::argb() const { return fvec4(a, r, g, b); }
	fvec4 fvec4::abgr() const { return fvec4(a, b, g, r); }

	fvec2 fvec4::xx() const { return fvec2(x); }
	fvec2 fvec4::yy() const { return fvec2(y); }
	fvec2 fvec4::zz() const { return fvec2(z); }
	fvec2 fvec4::ww() const { return fvec2(w); }

	fvec3 fvec4::xxx() const { return fvec3(x); }
	fvec3 fvec4::yyy() const { return fvec3(y); }
	fvec3 fvec4::zzz() const { return fvec3(z); }
	fvec3 fvec4::www() const { return fvec3(w); }

	fvec4 fvec4::xxxx() const { return fvec4(x); }
	fvec4 fvec4::yyyy() const { return fvec4(y); }
	fvec4 fvec4::zzzz() const { return fvec4(z); }
	fvec4 fvec4::wwww() const { return fvec4(w); }
}



















