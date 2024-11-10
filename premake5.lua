workspace "Partial"
   configurations { "Debug", "Release" }

   startproject "Demo"

   architecture "x64"

   targetdir ("bin/%{prj.name}/%{cfg.longname}")
   objdir ("bin-int/%{prj.name}/%{cfg.longname}")

project "glfw"
    kind "StaticLib"
    language "C"

    targetdir ("bin/%{prj.name}")
    objdir ("bin-int/%{prj.name}")

    files
    {
        "Partial/Dependencies/glfw/include/GLFW/glfw3.h",
        "Partial/Dependencies/glfw/include/GLFW/glfw3native.h",
        "Partial/Dependencies/glfw/src/internal.h",
        "Partial/Dependencies/glfw/src/platform.h",
        "Partial/Dependencies/glfw/src/mappings.h",
        "Partial/Dependencies/glfw/src/context.c",
        "Partial/Dependencies/glfw/src/init.c",
        "Partial/Dependencies/glfw/src/input.c",
        "Partial/Dependencies/glfw/src/monitor.c",
        "Partial/Dependencies/glfw/src/platform.c",
        "Partial/Dependencies/glfw/src/vulkan.c",
        "Partial/Dependencies/glfw/src/window.c",
        "Partial/Dependencies/glfw/src/egl_context.c",
        "Partial/Dependencies/glfw/src/osmesa_context.c",
        "Partial/Dependencies/glfw/src/null_platform.h",
        "Partial/Dependencies/glfw/src/null_joystick.h",
        "Partial/Dependencies/glfw/src/null_init.c",

        "Partial/Dependencies/glfw/src/null_monitor.c",
        "Partial/Dependencies/glfw/src/null_window.c",
        "Partial/Dependencies/glfw/src/null_joystick.c",

    }
    filter "system:linux"
        pic "On"

        systemversion "latest"
        staticruntime "On"

        files
        {
            "Partial/Dependencies/glfw/src/x11_init.c",
            "Partial/Dependencies/glfw/src/x11_monitor.c",
            "Partial/Dependencies/glfw/src/x11_window.c",
            "Partial/Dependencies/glfw/src/xkb_unicode.c",
            "Partial/Dependencies/glfw/src/posix_time.c",
            "Partial/Dependencies/glfw/src/posix_thread.c",
            "Partial/Dependencies/glfw/src/glx_context.c",
            "Partial/Dependencies/glfw/src/egl_context.c",
            "Partial/Dependencies/glfw/src/osmesa_context.c",
            "Partial/Dependencies/glfw/src/linux_joystick.c"
        }

        defines
        {
            "_GLFW_X11"
        
        }

        filter "system:windows"
            systemversion "latest"
            staticruntime "On"
    
        -- buildoptions{
        --     "/MT"
        -- }

        files
        {
            "Partial/Dependencies/glfw/src/win32_init.c",
            "Partial/Dependencies/glfw/src/win32_module.c",
            "Partial/Dependencies/glfw/src/win32_joystick.c",
            "Partial/Dependencies/glfw/src/win32_monitor.c",
            "Partial/Dependencies/glfw/src/win32_time.h",
            "Partial/Dependencies/glfw/src/win32_time.c",
            "Partial/Dependencies/glfw/src/win32_thread.h",
            "Partial/Dependencies/glfw/src/win32_thread.c",
            "Partial/Dependencies/glfw/src/win32_window.c",
            "Partial/Dependencies/glfw/src/wgl_context.c",
            "Partial/Dependencies/glfw/src/egl_context.c",
            "Partial/Dependencies/glfw/src/osmesa_context.c"
        }

        defines 
        { 
            "_GLFW_WIN32",
            "_CRT_SECURE_NO_WARNINGS"

        }

        filter "configurations:Debug"
            runtime "Debug"
            symbols "On"

        filter "configurations:Release"
            runtime "Release"
            optimize "On"


    project "glad"
       kind "StaticLib"
       language "C"
       staticruntime "off"

       targetdir ("bin/%{prj.name}/%{cfg.longname}")
       objdir ("bin-int/%{prj.name}/%{cfg.longname}")

       files
       {
          "Partial/Dependencies/glad/include/glad/glad.h",
          "Partial/Dependencies/glad/include/KHR/khrplatform.h",
          "Partial/Dependencies/glad/src/glad.c"
       }

       includedirs
       {
          "Partial/Dependencies/glad/include"
       }

       filter "system:windows"
          systemversion "latest"

       filter "configurations:Debug"
          runtime "Debug"
          symbols "on"

       filter "configurations:Release"
          runtime "Release"
          optimize "on"

project "stb"
   kind "StaticLib"
   language "C"

   targetdir ("bin/%{prj.name}/%{cfg.longname}")
   objdir ("bin-int/%{prj.name}/%{cfg.longname}")

   includedirs 
   {
      "Partial/Dependencies/stb"
   }

   files
   {
      "Partial/Dependencies/stb/stb_image.h", "Partial/Dependencies/stb_image/stb_image.cpp"
   }

   filter "system:windows"
      systemversion "latest"

   filter "configurations:Debug"
      runtime "Debug"
      symbols "on"

   filter "configurations:Release"
      runtime "Release"
      optimize "on"

project "Partial"
   kind "StaticLib"
   language "C++"

   targetdir ("bin/%{prj.name}/%{cfg.longname}")
   objdir ("bin-int/%{prj.name}/%{cfg.longname}")

   files
   {
      "Partial/src/**.cpp", "Partial/src/**.h" 
   }

   includedirs
   {
      "Partial/src",
      "Partial/Dependencies/glfw/include",
      "Partial/Dependencies/glad/include",
      "Partial/Dependencies/glm",
      "Partial/Dependencies/stb"
   }

   links {"glfw", "glad", "stb", "opengl32.lib"}

project "Demo"
    kind "ConsoleApp"

   targetdir ("bin/%{prj.name}/%{cfg.longname}")
   objdir ("bin-int/%{prj.name}/%{cfg.longname}")

   files
   {
       "Demo/src/**.h", "Demo/src/**.cpp"
   }

   includedirs 
   {
       "Demo/src",
       "Partial/src",
       "Partial/Dependencies/glfw/include",
       "Partial/Dependencies/glad/include",
       "Partial/Dependencies/glm"

   }

   links {"Partial"}