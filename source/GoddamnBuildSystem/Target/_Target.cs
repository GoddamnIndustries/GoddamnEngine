using System;

namespace GoddamnEngine.BuildSystem
{
    public enum TargetPlatform : byte
    {
        XBoxOne,
        PlayStation4,
        Windows,
        OSX,
        Linux,
        WindowsPhone,
        iOS,
        Android,
        HTML5,
    }   // enum TargetPlatform

    public enum TargetConfiguration : byte
    {
        Debug,
        Release
    }   // enum Configuration

    public abstract class Target
    {
        public static TargetPlatform Platform { get { return TargetPlatform.Windows; } }

        public static bool IsMonolithicBuild() 
        { 
            return (Target.IsForceMonolithicBuild() || !Target.IsDesktopPlatform()); 
        }

        public static bool IsForceMonolithicBuild() 
        { 
            return false; 
        }

        /// <summary> Returns true if specified platform is desktop</summary>
        public static bool IsDesktopPlatform(TargetPlatform Platform)
        {
            switch(Platform)   
            {
                case TargetPlatform.OSX: return true;
                case TargetPlatform.Linux: return true;
                case TargetPlatform.Windows: return true;
            }
        
            return false;
        }

        /// <summary> Returns true if target platform is desktop</summary>
        public static bool IsDesktopPlatform()
        {
            return Target.IsDesktopPlatform(Target.Platform);
        }

        /// <summary> Returns true if specified platform is mobile</summary>
        public static bool IsMobilePlatform(TargetPlatform Platform)
        {
            switch(Platform)   
            {
                case TargetPlatform.iOS: return true;
                case TargetPlatform.Android: return true;
                case TargetPlatform.WindowsPhone: return true;
            }
        
            return false;
        }

        /// <summary> Returns true if target platform is mobile</summary>
        public static bool IsMobilePlatform()
        {
            return Target.IsMobilePlatform(Target.Platform);
        }

        /// <summary> Returns true if specified platform is console</summary>
        public static bool IsConsolePlatform(TargetPlatform Platform)
        {
            switch (Platform)
            {
                case TargetPlatform.XBoxOne: return true;
                case TargetPlatform.PlayStation4: return true;
            }

            return false;
        }

        /// <summary> Returns true if target platform is console</summary>
        public static bool IsConsolePlatform()
        {
            return Target.IsConsolePlatform(Target.Platform);
        }

        /// <summary> Returns true if specified platform is Web</summary>
        public static bool IsWebPlatform(TargetPlatform Platform)
        {
            return (Platform == TargetPlatform.HTML5);
        }

        /// <summary> Returns true if target platform is Web</summary>
        public static bool IsWebPlatform()
        {
            return Target.IsWebPlatform(Target.Platform);
        }

        /// <summary> Returns true if specified platform`s native API is WinAPI</summary>
        public static bool IsWinAPIPlatform(TargetPlatform Platform)
        {
            switch (Platform)
            {
                case TargetPlatform.XBoxOne: return true;
                case TargetPlatform.Windows: return true;
                case TargetPlatform.WindowsPhone: return true;
            }

            return false;
        }

        /// <summary> Returns true if target platform`s native API is WinAPI</summary>
        public static bool IsWinAPIPlatform()
        {
            return Target.IsWinAPIPlatform(Target.Platform);
        }

        /// <summary> Returns true if specified platform`s native API is Cocoa</summary>
        public static bool IsCocoaPlatform(TargetPlatform Platform)
        {
            switch (Platform)
            {
                case TargetPlatform.iOS: return true;
                case TargetPlatform.OSX: return true;
            }

            return false;
        }

        /// <summary> Returns true if target platform`s native API is Cocoa</summary>
        public static bool IsCocoaPlatform()
        {
            return Target.IsCocoaPlatform(Target.Platform);
        }

        public static string GetPlatformStaticLibraryExtension(TargetPlatform Platform)
        {
            if (Target.IsWinAPIPlatform(Platform))
                return ".lib";

            return ".a";
        }

        public static string GetPlatformStaticLibraryExtension()
        {
            return Target.GetPlatformStaticLibraryExtension(Target.Platform);
        }

        public static string GetPlatformCodeName(TargetPlatform Platform)
        {
            switch(Platform)
            {
                case TargetPlatform.XBoxOne: return "XBox";
                case TargetPlatform.PlayStation4: return "PS4";
                case TargetPlatform.Windows: return "Win64";
                case TargetPlatform.OSX: return "OSX";
                case TargetPlatform.Linux: return "Linux";
                case TargetPlatform.WindowsPhone: return "WP";
                case TargetPlatform.iOS: return "iOS";
                case TargetPlatform.Android: return "Android";
                case TargetPlatform.HTML5: return "js";
            }

            throw new ArgumentException();
        }

        public abstract void GenerateProject(Project TheProject);
    }
}
