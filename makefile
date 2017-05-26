# Makefile for Orion-X3/Orion-X4/mx and derivatives
# Written in 2011
# This makefile is licensed under the WTFPL



WARNINGS		:= -Wno-unused-parameter -Wno-sign-conversion -Wno-padded -Wno-old-style-cast -Wno-conversion -Wno-shadow -Wno-missing-noreturn -Wno-unused-macros -Wno-switch-enum -Wno-deprecated -Wno-format-nonliteral -Wno-trigraphs -Wno-unused-const-variable


CLANGWARNINGS	:= -Wno-undefined-func-template -Wno-comma -Wno-nullability-completeness -Wno-redundant-move -Wno-nested-anon-types -Wno-gnu-anonymous-struct -Wno-reserved-id-macro -Wno-extra-semi -Wno-gnu-zero-variadic-macro-arguments -Wno-shift-sign-overflow -Wno-exit-time-destructors -Wno-global-constructors -Wno-c++98-compat-pedantic -Wno-documentation-unknown-command -Wno-weak-vtables -Wno-c++98-compat -Wno-float-equal -Wno-double-promotion -Wno-format-pedantic -Wno-missing-variable-declarations -Wno-implicit-fallthrough


SYSROOT			:= .
PREFIX			:= build
OUTPUTBIN		:= sotv

OUTPUT			:= $(SYSROOT)/$(PREFIX)/bin/$(OUTPUTBIN)

CC				?= "clang"
CXX				?= "clang++"

CXXSRC			:= $(shell find source -iname "*.cpp" -not -path "**/imgui_old/*")
CSRC			:= $(shell find source -iname "*.c" -not -path "**/imgui_old/*")

CXXOBJ			:= $(CXXSRC:.cpp=.cpp.o)
COBJ			:= $(CSRC:.c=.c.o)

CXXDEPS			:= $(CXXSRC:.cpp=.cpp.d)

NUMFILES		:= $$(($(words $(CXXSRC)) + $(words $(CSRC))))

LIBS			:= -lgl -lsdl2 -lsdl2_ttf -lsdl2_image -lglbinding
FRAMEWORKS		:= -framework GLUT -framework OpenGL -framework CoreVideo -framework Cocoa -framework CoreFoundation -lsdl2 -lsdl2_image -lsdl2_ttf -lglbinding

SANITISE		:=

CXXFLAGS		+= -std=c++14 -O0 -g -c -Wall -frtti -fexceptions -fno-omit-frame-pointer -Wno-old-style-cast
CFLAGS			+= -std=c11 -O0 -g -c -Wall -fno-omit-frame-pointer -Wno-overlength-strings

LDFLAGS			+= $(SANITISE)

INCLUDES 		:= -Isource/include -Isource/imgui


.DEFAULT_GOAL = osx
-include $(CXXDEPS)

.PHONY: clean build osx linux ci prep osxflags linuxflags

prep:
	@mkdir -p $(dir $(OUTPUT))

osxflags:
	$(eval CXXFLAGS += -march=native -fmodules -Weverything -Xclang -fcolor-diagnostics $(SANITISE) $(CLANGWARNINGS))
	$(eval CFLAGS += -fmodules -Xclang -fcolor-diagnostics $(SANITISE) $(CLANGWARNINGS))
	$(eval LDFLAGS += $(FRAMEWORKS))

osx: prep osxflags build run

linuxflags: LDFLAGS += $(LIBS)
linuxflags:

linux: prep linuxflags build

ci: linux

run: build
	@$(OUTPUT)

build: $(OUTPUT)
	# built

$(OUTPUT): $(CXXOBJ) $(COBJ)
	@echo "# linking"
	@$(CXX) -o $@ $(CXXOBJ) $(COBJ) $(LDFLAGS)


%.cpp.o: %.cpp
	@$(eval DONEFILES += "CPP")
	@printf "# compiling [$(words $(DONEFILES))/$(NUMFILES)] $<\n"
	@$(CXX) $(CXXFLAGS) $(WARNINGS) $(INCLUDES) -Isource/utf8rewind/include/utf8rewind -MMD -MP -MF $<.d -o $@ $<


%.c.o: %.c
	@$(eval DONEFILES += "C")
	@printf "# compiling [$(words $(DONEFILES))/$(NUMFILES)] $<\n"
	@$(CC) $(CFLAGS) $(WARNINGS) $(INCLUDES) -Isource/utf8rewind/include/utf8rewind -MMD -MP -MF $<.d -o $@ $<





# haha
clena: clean
clean:
	@rm -f $(OUTPUT)
	@find source -name "*.o" | xargs rm -f
	@find source -name "*.c.d" | xargs rm -f
	@find source -name "*.cpp.d" | xargs rm -f









