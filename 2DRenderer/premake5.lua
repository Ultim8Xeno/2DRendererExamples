project "OpenGLRenderer"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir "%{wks.location}/bin/%{cfg.system}/%{cfg.buildcfg}/%{cfg.architecture}/%{prj.name}"
	objdir "%{wks.location}/bin-int/%{cfg.system}/%{cfg.buildcfg}/%{cfg.architecture}/%{prj.name}"

	files
	{
		"**.hpp",
		"**.cpp",
		"**.h",
		"**.c"
	}

	includedirs
	{
		"dependencies/include"
	}

	links
	{
		"opengl32.lib",
		"glfw3.lib"
	}

	libdirs
	{
		"dependencies/lib"
	}

	filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
		symbols "on"

    filter "configurations:Release"
        defines "RELEASE"
        runtime "Release"
        optimize "on"