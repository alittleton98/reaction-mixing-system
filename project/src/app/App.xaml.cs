using System.ComponentModel;
using System.Configuration;
using System.Data;
using System.IO;
using System.Windows;
using System.Windows.Media.Animation;
using System.Windows.Threading;
using System.Diagnostics;

namespace NexusMixingSystem;

/// <summary>
/// Interaction logic for App.xaml
/// </summary>
/// 


public partial class App : Application
{
    private SplashScreen splash;
    private const int LoadingScreenDelayBetweenLoads = 500;
    private const int DEBUGGER_ATTACHMENT_DELAY = 30000;

    private string UserScriptPath = "";
    public static void WriteLog(string message)
    {
        Console.WriteLine("> " + message);
    }
    public App()
    {
        
    }

    private async Task InitializeClient()
    {
        if (splash == null)
        {
            return;
        }
        splash.SetStatus("Client Init...");
        bool success = await Task.Run(() => NMS_NativeCode.NexusClient_InitializeClient(""));
        if (success)
        {
            splash.SetStatus("Client Loaded!");
        }
        else
        {
            splash.SetStatus("FAILED TO INITIALIZE CLIENT");
            NMS_Utilities.DisplayLog("FAILED TO INITIALIZE CLIENT");
            await Task.Delay(500);
            splash.SetStatus("Closing...");
            NMS_Utilities.DisplayLog("Closing...");
            MessageBox.Show("Base initialization failed. Click Ok to close.", "Close", MessageBoxButton.OK, MessageBoxImage.Exclamation);
            Shutdown();
        }
    }

    private async Task InitializeSoundEngine()
    {
        if (splash == null) 
        {
            return;
        }
        splash.SetStatus("Sound Engine Init...");
        await Task.Run(() => NMS_NativeCode.NexusSoundEngine_InitializeSoundEngine());
        splash.SetStatus("Sound Engine Loaded!");
    }

    async void Application_Startup(object sender, StartupEventArgs e)
    {
        ShutdownMode = ShutdownMode.OnExplicitShutdown;
        splash = new SplashScreen();
        splash.Show();
        splash.SetStatus($"Waiting for debugger...{DEBUGGER_ATTACHMENT_DELAY / 1000}");
        int debuggerWaitInSeconds = (DEBUGGER_ATTACHMENT_DELAY / 1000);
        int debuggerCheckFrequency = debuggerWaitInSeconds; // Default to once per second
        for (int i = 0; i < debuggerCheckFrequency; i++)
        {
            splash.SetStatus($"Waiting for debugger...{debuggerWaitInSeconds - ((debuggerWaitInSeconds/debuggerCheckFrequency) * i)}");
            await Task.Delay(DEBUGGER_ATTACHMENT_DELAY / debuggerCheckFrequency);
            if (Debugger.IsAttached)
            {
                splash.SetStatus("DEBUGGER ATTACHED");
                break;
            }
            else
            {
                if (NMS_NativeCode.IsDebuggerPresent())
                {
                    splash.SetStatus("DEBUGGER ATTACHED");
                    break;
                }
            }
        }
        StdoutCapture.Start();
#if DEBUG
        if (NMS_NativeCode.AllocConsole())
        {
            // Rebind in case Console was touched before the console existed
            var stdout = new StreamWriter(Console.OpenStandardOutput()) { AutoFlush = true };
            var stderr = new StreamWriter(Console.OpenStandardError()) { AutoFlush = true };
            Console.SetOut(stdout);
            Console.SetError(stderr);
            Console.Title = "Nexus Mixing System Log";
        }

        Console.WriteLine("Nexus Mixing Desk Command Line Interface \nalittl3ton13\nVersion: " + BuildDefines.NmsVersionString);
        if (Debugger.IsAttached)
        {
            Console.WriteLine("C# DEBUGGER ATTACHED");
        }
        else
        {
            if (NMS_NativeCode.IsDebuggerPresent())
            {
                Console.WriteLine("C++ DEBUGGER ATTACHED");
            }
            else
            {
                Console.WriteLine("NO DEBUGGER ATTACHED");
            }
        }
        Console.WriteLine("===========================================");
        NMS_NativeCode.NexusClient_AttachConsoleOutput();
#endif
        splash.SetStatus("Client Initialization");

        await InitializeClient();
        
        if (e.Args.Length > 0)
        {
            if (e.Args[0] == "-script" && e.Args.Length > 1)
            {
                UserScriptPath = e.Args[1];
            }
        }
        else 
        {
            UserScriptPath = BuildDefines.ScriptOutputDirectory + BuildDefines.ScriptDefaultName + BuildDefines.ScriptFileExtension;
        }

        // Load core modules and data


        // Initialize feature modules
        //await InitializeSoundEngine();
        await Task.Delay(LoadingScreenDelayBetweenLoads);

        // Ready main window
        var main = new MainWindow();
        MainWindow = main;
        ShutdownMode = ShutdownMode.OnMainWindowClose;
        
        // clean up splash
        splash.SetStatus("Done!");
        await Task.Delay(LoadingScreenDelayBetweenLoads);
        splash.Close();
        main.Show();
    }


}




