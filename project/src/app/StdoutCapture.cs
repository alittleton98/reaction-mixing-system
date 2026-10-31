using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using Microsoft.Win32.SafeHandles;

namespace NexusMixingSystem;

/// <summary>
/// Redirects the process-wide stdout (C runtime fd 1 and the Win32 std handle) into a pipe so that
/// anything printed by the native libraries, as well as Console.Out from C#, is raised as lines.
/// </summary>
public static class StdoutCapture
{
    private const int StdOutputHandle = -11;
    private const int StdErrorHandle = -12;
    private const int StdOutFd = 1;
    private const int StdErrFd = 2;
    private const int IoNoBuf = 0x0004;

    private static readonly object gate = new();
    private static readonly List<string> history = new();
    private static bool started;

    /// <summary>Raised on a background thread for every complete line.</summary>
    public static event Action<string>? LineWritten;

    [DllImport("kernel32.dll", SetLastError = true)]
    [return: MarshalAs(UnmanagedType.Bool)]
    private static extern bool CreatePipe(out SafeFileHandle readPipe, out SafeFileHandle writePipe, nint attributes, uint size);

    [DllImport("kernel32.dll", SetLastError = true)]
    [return: MarshalAs(UnmanagedType.Bool)]
    private static extern bool SetStdHandle(int stdHandle, SafeFileHandle handle);

    [DllImport("ucrtbase.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern int _open_osfhandle(nint osfHandle, int flags);

    [DllImport("ucrtbase.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern int _dup2(int fdSrc, int fdDst);

    [DllImport("ucrtbase.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern nint __acrt_iob_func(uint index);

    [DllImport("ucrtbase.dll", CallingConvention = CallingConvention.Cdecl)]
    private static extern int setvbuf(nint stream, nint buffer, int mode, nuint size);

    /// <summary>Lines captured so far, for consumers that attach late.</summary>
    public static string[] GetHistory()
    {
        lock (gate)
        {
            return history.ToArray();
        }
    }

    public static void Start()
    {
        if (started)
        {
            return;
        }

        if (!CreatePipe(out SafeFileHandle readHandle, out SafeFileHandle writeHandle, 0, 0))
        {
            return;
        }

        // The CRT fd takes ownership of the OS handle once opened, so hand it a duplicate-free raw value
        // and keep our SafeFileHandle alive for the process lifetime.
        int pipeFd = _open_osfhandle(writeHandle.DangerousGetHandle(), 0);
        if (pipeFd < 0 || _dup2(pipeFd, StdOutFd) != 0)
        {
            return;
        }

        // stderr shares the same pipe so output stays interleaved in order.
        if (_dup2(pipeFd, StdErrFd) != 0)
        {
            return;
        }

        SetStdHandle(StdOutputHandle, writeHandle);
        SetStdHandle(StdErrorHandle, writeHandle);

        // Native streams are fully buffered when not attached to a console; make them unbuffered so lines appear immediately.
        setvbuf(__acrt_iob_func(1), 0, IoNoBuf, 0);
        setvbuf(__acrt_iob_func(2), 0, IoNoBuf, 0);

        // Console.Out / Console.Error in C# write straight into the same pipe.
        var writer = new StreamWriter(new FileStream(new SafeFileHandle(writeHandle.DangerousGetHandle(), false), FileAccess.Write, 1, false),
                                      new UTF8Encoding(false)) { AutoFlush = true };
        var synced = TextWriter.Synchronized(writer);
        Console.SetOut(synced);
        Console.SetError(synced);

        started = true;

        var reader = new Thread(() => ReadLoop(readHandle))
        {
            IsBackground = true,
            Name = "StdoutCapture"
        };
        reader.Start();
    }

    private static void ReadLoop(SafeFileHandle readHandle)
    {
        using var stream = new FileStream(readHandle, FileAccess.Read, 1, false);
        using var sr = new StreamReader(stream, Encoding.UTF8);
        string? line;
        while ((line = sr.ReadLine()) != null)
        {
            lock (gate)
            {
                history.Add(line);
                if (history.Count > 5000)
                {
                    history.RemoveRange(0, history.Count - 5000);
                }
            }
            LineWritten?.Invoke(line);
        }
    }
}
