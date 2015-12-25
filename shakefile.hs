-- shakefile.hs
-- Copyright (c) 2014 - The Foreseeable Future, zhiayang@gmail.com
-- Licensed under the Apache License Version 2.0.

{-# OPTIONS_GHC -fno-warn-tabs #-}

import System.Exit
import System.IO.Unsafe()
import Data.IORef()
import Data.Maybe()
import Development.Shake
import Development.Shake.Command
import Development.Shake.FilePath
import Development.Shake.Util

main :: IO()

buildDir	= "build" :: [Char]
binDir		= "bin" :: [Char]
outputBin	= "connect" :: [Char]

finalOutput	= buildDir </> binDir </> outputBin :: [Char]


disableWarn	= "-Wno-unused-parameter -Wno-sign-conversion -Wno-padded -Wno-c++98-compat -Wno-weak-vtables -Wno-documentation-unknown-command -Wno-old-style-cast -Wno-c++98-compat-pedantic -Wno-conversion -Wno-shadow -Wno-global-constructors -Wno-exit-time-destructors -Wno-missing-noreturn -Wno-unused-macros -Wno-switch-enum -Wno-deprecated -Wno-shift-sign-overflow -Wno-format-nonliteral -Wno-gnu-zero-variadic-macro-arguments -Wno-trigraphs -Wno-reserved-id-macro -Wno-float-equal"



main = shakeArgs shakeOptions { shakeFiles = "build" } $ do
	want ["run"]

	phony "run" $ do
		need [finalOutput]
		cmd Shell (buildDir </> binDir </> outputBin)

	phony "build" $ do
		need [finalOutput]

	phony "clean" $ do
		putNormal "Cleaning files"
		removeFilesAfter "source" ["//*.o"]
		removeFilesAfter "build" ["bin/*"]


	finalOutput %> \out -> do
		cs <- getDirectoryFiles "" ["source//*.cpp", "source//*.c"]
		let os = [c ++ ".o" | c <- cs]
		need os


		let linkerFlags' = "-lSDL2 -lSDL2_image -lSDL2_ttf -lglfw3 -framework OpenGL -framework GLUT -framework CoreVideo -framework Cocoa "
		let linkerFlags = linkerFlags' ++ "-g -O2 -Wall -stdlib=libc++ -std=gnu++1y -I{src}/include"

		cmd Shell "clang++ -o" [out] [linkerFlags] os


	"source//*.cpp.o" %> \out -> do
		let c = dropExtension out
		let m = out ++ ".m"

		let cxxFlags = "-std=gnu++1y -g -Wall -Weverything " ++ disableWarn ++ " -frtti -fexceptions -fno-omit-frame-pointer -Isource/include -Isource/imgui"
		() <- cmd Shell "clang++ -c" [c] [cxxFlags] "-o" [out] "-MMD -MF" [m]
		needMakefileDependencies m



	"source//*.c.o" %> \out -> do
		let c = dropExtension out
		let m = out ++ ".m"

		let cxxFlags = "-std=gnu11 -g -Wall -Weverything " ++ disableWarn ++ " -fno-omit-frame-pointer -Isource/include -Isource/imgui"
		() <- cmd Shell "clang -c" [c] [cxxFlags] "-o" [out] "-MMD -MF" [m]
		needMakefileDependencies m



















