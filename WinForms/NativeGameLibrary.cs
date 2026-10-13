using System.Runtime.InteropServices;

namespace KatanaEngine.WinForms;

/// <summary>
/// The C exports a hosted KatanaEngine game DLL provides (see Source/Core/Hosting/HostExports.h).
/// </summary>
internal sealed class NativeGameLibrary
{
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    public delegate void MessageCallback(IntPtr utf8Message);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate int StartFunction(IntPtr parentWindow, MessageCallback onMessage);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate void ResizeFunction(int width, int height);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate void PostFunction([MarshalAs(UnmanagedType.LPUTF8Str)] string message);

    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate void StopFunction();

    private readonly StartFunction _start;
    private readonly ResizeFunction _resize;
    private readonly PostFunction _post;
    private readonly StopFunction _stop;

    private NativeGameLibrary(IntPtr library)
    {
        _start = GetExport<StartFunction>(library, "KatanaHost_Start");
        _resize = GetExport<ResizeFunction>(library, "KatanaHost_Resize");
        _post = GetExport<PostFunction>(library, "KatanaHost_Post");
        _stop = GetExport<StopFunction>(library, "KatanaHost_Stop");
    }

    /// <summary>
    /// Loads the game DLL (relative paths resolve against the app's base directory).
    /// The library stays loaded for the lifetime of the process: Allegro cannot be
    /// re-initialised after being unloaded.
    /// </summary>
    public static NativeGameLibrary Load(string path)
    {
        var fullPath = Path.IsPathRooted(path) ? path : Path.Combine(AppContext.BaseDirectory, path);
        return new NativeGameLibrary(NativeLibrary.Load(fullPath));
    }

    public bool Start(IntPtr parentWindow, MessageCallback onMessage) => _start(parentWindow, onMessage) != 0;
    public void Resize(int width, int height) => _resize(width, height);
    public void Post(string message) => _post(message);
    public void Stop() => _stop();

    private static T GetExport<T>(IntPtr library, string name) where T : Delegate =>
        Marshal.GetDelegateForFunctionPointer<T>(NativeLibrary.GetExport(library, name));
}
