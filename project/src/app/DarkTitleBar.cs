using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Interop;

namespace ReactionMixingSystem;

/// <summary>
/// Switches a window's native title bar to the Windows dark theme.
/// </summary>
static class DarkTitleBar
{
    // 20 on Windows 10 2004+ / 11; 19 on earlier Windows 10 builds
    const int DWMWA_USE_IMMERSIVE_DARK_MODE = 20;
    const int DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1 = 19;

    [DllImport("dwmapi.dll")]
    static extern int DwmSetWindowAttribute(IntPtr hwnd, int attribute, ref int value, int size);

    public static void Apply(Window window)
    {
        var hwnd = new WindowInteropHelper(window).Handle;
        int enabled = 1;
        if (DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, ref enabled, sizeof(int)) != 0)
            DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1, ref enabled, sizeof(int));
    }
}
