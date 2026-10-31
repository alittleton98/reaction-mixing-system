using System.Runtime.InteropServices;

//internal static partial class NexusNativeCode
//{
//    private const string nativeLibrary = "";

//    [DllImport(nativeLibrary, CallingConvention = CallingConvention.Cdecl)]
//    [return: MarshalAs(UnmanagedType.U1)]
//    private static extern bool <Native Module Name>_Initialize();
//}
internal static partial class NMS_NativeCode
{
    // Windows 
    [DllImport("kernel32.dll", SetLastError = true)]
    public static extern bool AllocConsole();

    [DllImport("kernel32.dll", SetLastError = true)]
    public static extern bool FreeConsole();

    [DllImport("kernel32.dll")]
    public static extern bool IsDebuggerPresent();

    // Client
    [DllImport("nms_common.dll")]
    [UnmanagedCallConv(CallConvs = new[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
    public static extern void NexusClient_AttachConsoleOutput();

    [DllImport("nms_common.dll")]
    [UnmanagedCallConv(CallConvs = new[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
    [return: MarshalAs(UnmanagedType.U1)]
    public static extern bool NexusClient_InitializeClient(string path);

    // Sound Engine
    [DllImport("nms_soundengine")]
    [UnmanagedCallConv(CallConvs = new[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
    [return: MarshalAs(UnmanagedType.U1)]
    public static extern bool NexusSoundEngine_InitializeSoundEngine();
}
