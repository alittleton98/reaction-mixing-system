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

    [DllImport("nms_soundengine")]
    [UnmanagedCallConv(CallConvs = new[] { typeof(System.Runtime.CompilerServices.CallConvCdecl) })]
    [return: MarshalAs(UnmanagedType.U1)]
    public static extern bool NexusSoundEngine_InitializeSoundEngine();
}
